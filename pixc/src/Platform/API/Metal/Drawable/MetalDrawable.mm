#include "pixcpch.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"

#include "Platform/Metal/MetalRendererUtils.h"

#include "Platform/Metal/Buffer/MetalVertexBuffer.h"
#include "Platform/Metal/Buffer/MetalIndexBuffer.h"

#include "Platform/Metal/Shader/MetalShader.h"

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

namespace pixc {

/**
 * @brief An internal structure encapsulating the Metal-specific state of a `MetalDrawable`.
 */
struct MetalDrawable::DrawableState {
    ///< The compiled Metal render pipeline state.
    id<MTLRenderPipelineState> PipelineState;
    
    ///< Describes the rendering pipeline.
    MTLRenderPipelineDescriptor* PipelineDescriptor;
    ///< Describes the layout of vertex data.
    MTLVertexDescriptor* VertexDescriptor;
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
    
    // Allocate memory for the descriptors
    m_State->PipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    m_State->VertexDescriptor = [[MTLVertexDescriptor alloc] init];
}

/**
 * @brief Destroys the Metal drawable object.
 */
MetalDrawable::~MetalDrawable()
{
    // De-allocate memory
    [m_State->PipelineDescriptor release];
    [m_State->VertexDescriptor release];
}

/**
 * @brief Binds the drawble  to the current Metal rendering context.
 */
void MetalDrawable::Bind() const
{
    // Get the command encoder to encode rendering commands into the buffer
    id<MTLRenderCommandEncoder> encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());
    
    // Define the state of the pipeline if not yet defined
    if (!m_State->PipelineState)
        SetPipelineState();
    // Set the pipeline state
    [encoder setRenderPipelineState:m_State->PipelineState];
    
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
    auto* metalShader = dynamic_cast<MetalShader*>(m_Shader.get());
    PIXEL_CORE_ASSERT(metalShader, "Invalid shader cast - not a Metal shader!");
    metalShader->BindUniformBuffers();
}

/**
 * @brief Creates and configures the Metal render pipeline state.
 */
void MetalDrawable::SetPipelineState() const
{
    // Check that a shader has been defined in the drawable object
    PIXEL_CORE_ASSERT(m_Shader, "Shader needs to be defined in drawable object for Metal API!");
    
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
    
    // Dynamic cast the shader to a Metal shader
    auto* metalShader = dynamic_cast<MetalShader*>(m_Shader.get());
    PIXEL_CORE_ASSERT(metalShader, "Invalid shader cast - not a Metal shader!");
    
    // Get Metal functions
    id<MTLFunction> vertexFunction = reinterpret_cast<id<MTLFunction>>(metalShader->GetVertexFunction());
    id<MTLFunction> fragmentFunction = reinterpret_cast<id<MTLFunction>>(metalShader->GetFragmentFunction());
    
    // Assign to pipeline descriptor
    m_State->PipelineDescriptor.vertexFunction = vertexFunction;
    m_State->PipelineDescriptor.fragmentFunction = fragmentFunction;
    m_State->PipelineDescriptor.vertexDescriptor = m_State->VertexDescriptor;
    
    // TODO: needs to be defined by the framebuffer information directly.
    m_State->PipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
    m_State->PipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    
    // Define the pipeline state
    NSError* error = nil;
    m_State->PipelineState = [device newRenderPipelineStateWithDescriptor:m_State->PipelineDescriptor error:&error];
    PIXEL_CORE_ASSERT(!error, "Failed to define the pipeline state!");
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
 * @brief Retrieves the Metal render pipeline state.
 */
void* MetalDrawable::GetPipelineState() const
{
    return reinterpret_cast<void*>(m_State->PipelineState);
}

} // namespace pixc
