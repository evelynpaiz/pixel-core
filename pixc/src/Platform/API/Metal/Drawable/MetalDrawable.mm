#include "pixcpch.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"

#include "Platform/Metal/MetalRendererUtils.h"
#include "Platform/Metal/Texture/MetalTextureUtils.h"

#include "Platform/Metal/Buffer/MetalVertexBuffer.h"
#include "Platform/Metal/Buffer/MetalIndexBuffer.h"

#include "Platform/Metal/Shader/MetalShader.h"

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

namespace pixc {

// Alias for a map that associates uniform names with their corresponding Metal buffers.
// NOTE: This represents all uniform buffers for a single shader.
using UniformBufferMap = std::unordered_map<std::string, id<MTLBuffer>>;

/**
 * @brief An internal structure encapsulating the Metal-specific state of a `MetalDrawable`.
 */
struct MetalDrawable::DrawableState
{
    ///< Describes the layout of vertex data.
    MTLVertexDescriptor* VertexDescriptor;
    
    ///< Cached pipeline states for different framebuffer attachment configurations.
    std::unordered_map<AttachmentSpecification, id<MTLRenderPipelineState>> PipelineStates;
    
    ///< Cached per-shader, per-uniform named buffer storage.
    ///  Key: Shader name → map of uniform name → MTLBuffer.
    std::unordered_map<std::string, UniformBufferMap> UniformBuffers;
};

/**
 * @brief Creates a drawable object for the Metal pipeline.
 */
MetalDrawable::MetalDrawable() : Drawable()
{
    // Get the Metal graphics context and save it
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    PIXEL_CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
    
    // Initalize the drawing state
    m_State = std::make_shared<DrawableState>();
    
    // Allocate memory for the vertex descriptors
    m_State->VertexDescriptor = [[MTLVertexDescriptor alloc] init];
}

/**
 * @brief Destroys the Metal drawable object.
 */
MetalDrawable::~MetalDrawable()
{
    // De-allocate memory for the vertex descriptors
    [m_State->VertexDescriptor release];
}

/**
 * @brief Binds the drawble  to the current Metal rendering context.
 */
void MetalDrawable::Bind() const
{
    // Get the command encoder to encode rendering commands into the buffer
    auto encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());
    
    // Define the vertex buffers in the command encoder
    for (size_t i = 0; i < m_VertexBuffers.size(); ++i)
    {
        auto metalVertexBuffer = std::dynamic_pointer_cast<MetalVertexBuffer>(m_VertexBuffers[i]);
        PIXEL_CORE_ASSERT(metalVertexBuffer, "Invalid buffer cast - not a Metal index buffer!");
        
        id<MTLBuffer> vertexBuffer = reinterpret_cast<id<MTLBuffer>>(metalVertexBuffer->GetBuffer());
        [encoder
            setVertexBuffer:vertexBuffer
            offset:0
            atIndex:static_cast<NSUInteger>(i)
        ];
    }
    
    // Define the uniforms in the command encoder
    BindUniformBuffers();
}

/**
 * @brief Sets the shader used for shading.
 *
 * @param shader The shader program.
 */
void MetalDrawable::SetShader(const std::shared_ptr<Shader>& shader)
{
    // Define the current shader
    m_Shader = shader;
    // Define the uniforms
    InitUniformBuffers();
}

/**
 * @brief Retrieves or creates a Metal render pipeline state.
 *
 * @return A pointer to the Metal render pipeline state.
 */
void* MetalDrawable::GetOrCreateRenderPipelineState(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    // Determine attachment specification from framebuffer (fallback to generic if null)
    static const AttachmentSpecification kGenericAttachments = {
        { TextureType::TEXTURE2D, TextureFormat::RGBA8 },
        { TextureType::TEXTURE2D, TextureFormat::DEPTH16 }
    };
    
    @autoreleasepool
    {
        auto attachments = framebuffer ? framebuffer->GetSpec().AttachmentsSpec : kGenericAttachments;
        
        // Return cached pipeline state if already created
        auto it = m_State->PipelineStates.find(attachments);
        if (it != m_State->PipelineStates.end())
            return reinterpret_cast<void*>(it->second);
        
        // Get Metal device
        id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        
        // Dynamic cast the shader to a Metal shader
        auto* metalShader = dynamic_cast<MetalShader*>(m_Shader.get());
        PIXEL_CORE_ASSERT(metalShader, "Invalid shader cast - not a Metal shader!");
        
        // Get Metal functions
        id<MTLFunction> vertexFunction = reinterpret_cast<id<MTLFunction>>(metalShader->GetVertexFunction());
        id<MTLFunction> fragmentFunction = reinterpret_cast<id<MTLFunction>>(metalShader->GetFragmentFunction());
        
        // Create and configure pipeline descriptor
        MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        
        descriptor.vertexFunction = vertexFunction;
        descriptor.fragmentFunction = fragmentFunction;
        descriptor.vertexDescriptor = m_State->VertexDescriptor;
        
        // Configure color attachment(s)
        size_t colorAttachmentCount = framebuffer ? framebuffer->GetColorAttachments().size() : 1;
        for (size_t i = 0; i < colorAttachmentCount; ++i)
        {
            descriptor.colorAttachments[i].pixelFormat = framebuffer ?
            utils::textures::mtl::ToMetalPixelFormat(framebuffer->GetColorAttachment((unsigned int)i)->GetSpecification().Format) :
            MTLPixelFormatRGBA8Unorm;
        }
        // Configure depth attachment format
        if(!framebuffer || framebuffer->GetActiveRenderTargets().Depth)
        {
            descriptor.depthAttachmentPixelFormat = framebuffer ?
            utils::textures::mtl::ToMetalPixelFormat(framebuffer->GetDepthAttachment()->GetSpecification().Format) :
            MTLPixelFormatDepth16Unorm;
        }
        
        // Create the pipeline state
        NSError* error = nil;
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:descriptor error:&error];
        [descriptor release];
        
        PIXEL_CORE_ASSERT(!error, "Failed to create Metal render pipeline state!");
        
        // Cache and return the pipeline state
        m_State->PipelineStates[attachments] = pipelineState;
        return reinterpret_cast<void*>(pipelineState);
    }
}

/**
 * @brief Sets the vertex attribute layout for the drawable.
 *
 * @param vbo The vertex buffer whose layout will be used to set the vertex attributes.
 */
void MetalDrawable::SetVertexAttributes(const std::shared_ptr<VertexBuffer> &vbo)
{
    // Check if the vertex buffer has a layout defined
    PIXEL_CORE_ASSERT(!vbo->GetLayout().IsEmpty(), "Vertex buffer has no layout!");
    
    // Get the index of the vertex buffer
    int indexVertexBuffer = GetVertexBufferIndex(vbo);
    PIXEL_CORE_ASSERT(indexVertexBuffer >= 0, "Vertex buffer not defined inside the drawable!");
    
    // Describe the vertex
    const auto& layout = vbo->GetLayout();
    for (const auto& name : layout.GetBufferOrder())
    {
        // Define the vertex attribute
        auto& element = layout.Get(name);
        auto *attribute = m_State->VertexDescriptor.attributes[m_Index];
        attribute.format = utils::graphics::mtl::ToMetalFormat(element.Type);
        attribute.offset = element.Offset;
        attribute.bufferIndex = indexVertexBuffer;
        m_Index++;
    }
    
    auto *layouts = m_State->VertexDescriptor.layouts[indexVertexBuffer];
    layouts.stride = layout.GetStride();
}

/**
 * @brief Initializes Metal uniform buffers for each uniform block in the shader.
 *
 * @note  Assumes that `m_Uniforms` has been populated with valid `UniformLayout`
 *        objects and that the `DataElement` objects within the layouts have
 *        accurate data pointers (`Data`), sizes (`Size`), and offsets (`Offset`).
 */
void MetalDrawable::InitUniformBuffers()
{
    // If the shader already has buffers cached for this drawable, skip
    if (m_State->UniformBuffers.find(m_Shader->GetName()) != m_State->UniformBuffers.end())
        return;
    
    @autoreleasepool
    {
        // Retrieve the Metal device from the rendering context
        auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        // Iterate over each uniform block layout defined for the shader
        auto shader = std::dynamic_pointer_cast<MetalShader>(m_Shader);
        
        // Create a new map for this shader's uniform buffers
        std::unordered_map<std::string, id<MTLBuffer>> buffers;
        
        for (auto& [name, uniform] : shader->m_Uniforms)
        {
            // Determine the required buffer size for this uniform layout
            uint32_t stride = uniform.GetStride();
            // Create a new Metal buffer for this uniform block
            id<MTLBuffer> buffer = [device
                                    newBufferWithLength:stride
                                    options:MTLResourceStorageModeShared
            ];
            // Store the buffer in the shader source's uniform buffer list
            buffers[name] = buffer;
        }
        // Store in the per-shader cache
        m_State->UniformBuffers.emplace(m_Shader->GetName(), std::move(buffers));
        
    } // autoreleasepool
}

/**
 * @brief Binds all uniform buffers to the GPU pipeline.
 *
 * @note  Assumes that `m_Uniforms` has been populated with valid `UniformLayout`
 *        objects and that the `DataElement` objects within the layouts have
 *        accurate data pointers (`Data`), sizes (`Size`), and offsets (`Offset`).
 */
void MetalDrawable::BindUniformBuffers() const
{
    // Update the buffers with the current data
    UpdateUniformBuffers();
    
    // Get the current command encoder
    auto encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());
    
    // Iterate over all uniform groups and their layouts stored
    auto shader = std::dynamic_pointer_cast<MetalShader>(m_Shader);
    for (const auto& [uniform, layout] : shader->m_Uniforms)
    {
        // Retrieve the binding index specified in the layout, which indicates
        // the slot the buffer should be bound to in the shader
        int32_t index = layout.GetIndex();
        
        // Get the Metal buffer associated with the current uniform group by index
        id<MTLBuffer> buffer = m_State->UniformBuffers[m_Shader->GetName()][uniform];
        PIXEL_CORE_ASSERT(buffer, "Uniform buffer not defined!");

        // Iterate over all shader stages that use this uniform layout (e.g., vertex, fragment, etc.)
        for (const auto& type : layout.GetShaderType())
        {
            // Bind the buffer to the appropriate shader stage at the specified index
            switch (type)
            {
                case ShaderType::VERTEX:
                    [encoder setVertexBuffer:buffer offset:0 atIndex:index];
                    break;
                case ShaderType::FRAGMENT:
                    [encoder setFragmentBuffer:buffer offset:0 atIndex:index];
                    break;
                default:
                    PIXEL_CORE_WARN("Unsupported shader type for uniform: {}", uniform);
                    break;
            }
        }
    }
}

/**
 * @brief Updates a specific uniform buffer if any of its members have changed.
 *
 *@param name The name of the uniform to be updated.
 *
 * @note  Assumes that `m_Uniforms` has been populated with valid `UniformLayout`
 *        objects and that the `DataElement` objects within the layouts have
 *        accurate data pointers (`Data`), sizes (`Size`), and offsets (`Offset`).
 */
void MetalDrawable::UpdateUniformBuffers() const
{
    // Iterate over all uniform groups and their layouts stored
    auto shader = std::dynamic_pointer_cast<MetalShader>(m_Shader);
    for (const auto& [uniform, layout] : shader->m_Uniforms)
    {
        // Get the source and destination buffer associated with the current uniform
        id<MTLBuffer> src = reinterpret_cast<id<MTLBuffer>>(layout.GetBufferOfData());
        id<MTLBuffer> dst = m_State->UniformBuffers[m_Shader->GetName()][uniform];
        
        PIXEL_CORE_ASSERT(src && dst, "Uniform buffer not defined!");
        
        NSUInteger copySize = std::min(src.length, dst.length);
        std::memcpy(dst.contents, src.contents, copySize);
    }
}

} // namespace pixc
