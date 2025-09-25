#include "pixcpch.h"
#include "Platform/Metal/MetalRendererAPI.h"

#include "Platform/Metal/Buffer/MetalIndexBuffer.h"
#include "Platform/Metal/Texture/MetalTexture.h"
#include "Platform/Metal/Shader/MetalShader.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"
#include "Platform/Metal/Buffer/MetalFrameBuffer.h"

#include "Platform/Metal/MetalRendererUtils.h"
#include "Platform/Metal/MetalStateDescriptor.h"

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

namespace pixc {

/**
 * @brief Represents the rendering target resources.
 */
struct MetalRenderTarget
{
    ///< The color attachment texture (often from the drawable).
    id<MTLTexture> ColorAttachment;
    ///< The depth attachment texture.
    id<MTLTexture> DepthAttachment;
};

/**
 * @brief Caches various GPU state objects for reuse.
 */
struct MetalRenderCache
{
    ///< Cache of depth-stencil states keyed by depth descriptor.
    std::unordered_map<MetalDepthDescriptor, id<MTLDepthStencilState>> Depth;
};

/**
 * @brief Holds the core Metal objects required for rendering within the MetalRendererAPI.
 */
struct MetalRendererAPI::MetalRendererState
{
    ///< Render target resources.
    MetalRenderTarget RenderTarget;
    
    ///< Viewport desccriptor.
    MTLViewport Viewport;
    ///< Clear color.
    MTLClearColor ClearColor;
    
    ///< Rendering configuration.
    MetalRenderDescriptor RenderDescriptor;
    ///< Depth configuration.
    MetalDepthDescriptor DepthDescriptor;
    
    /// Cache of render-related GPU states (e.g., depth-stencil).
    MetalRenderCache Cache;
};

/**
 * @brief Initializes the Metal rendering API.
 *
 * This method handles the Metal-specific initialization procedures.
 */
void MetalRendererAPI::Init()
{
    // Get the Metal graphics context and save it
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    PIXEL_CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
    
    // Initialize the Metal state object for managing Metal resources
    m_State = std::make_shared<MetalRendererState>();
}

/**
 * @brief Define the color to clear the color buffer.
 *
 * @param color The color to use.
 */
void MetalRendererAPI::SetClearColor(const glm::vec4& color)
{
    // Set the clear color
    m_State->ClearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
}

/**
 * @brief Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
 void MetalRendererAPI::SetViewport(const uint32_t x, const uint32_t y,
                                    const uint32_t width, const uint32_t height)
 {
     // Get the size of the current drawable
     glm::vec2 drawableSize = m_Context->GetBackbufferSize();
     
     // Define the new size of the viewport
     m_State->Viewport.originX = static_cast<double>(x);
     m_State->Viewport.originY = static_cast<double>(drawableSize.y - (y + height));
     m_State->Viewport.width = static_cast<double>(width);
     m_State->Viewport.height = static_cast<double>(height);
     
     m_State->Viewport.znear = 0.0f;
     m_State->Viewport.zfar = 1.0f;
     
     // Update the viewport size in the graphics context
     m_Context->UpdateViewport(&m_State->Viewport);
 }

/**
 * @brief Enable or disable depth testing.
 *
 * @param enabled Pass true to enable depth testing, false to disable it.
 */
void MetalRendererAPI::EnableDepthTesting(const bool enabled)
{
    // Enable/disable the depth state
    m_State->DepthDescriptor.Enabled = enabled;
    
    // Update the current depth stencil state
    m_Context->SetDepthStencilState(GetOrCreateDepthState());
}

/**
 * @brief Set the depth comparison function used during depth testing.
 *
 * @param function The depth function to use (e.g., Less, LessEqual, Greater, Always).
 */
void MetalRendererAPI::SetDepthFunction(const DepthFunction function)
{
    if (function == DepthFunction::None)
        return;
    // Change the depth function
    m_State->DepthDescriptor.Function = function;
    
    // Update the current depth stencil state
    m_Context->SetDepthStencilState(GetOrCreateDepthState());
}

/**
 * @brief Initialize a new rendering pass.
 *
 * @param framebuffer Buffer to hold to result of the rendered pass.
 */
void MetalRendererAPI::BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    // Update the information of the currently bound framebuffer
    RendererAPI::BeginRenderPass(framebuffer);
    
    // Initialize the command buffer
    m_Context->InitCommandBuffer();
}

/**
 * @brief Finalize the current rendering pass.
 */
void MetalRendererAPI::EndRenderPass()
{
    // End the encoding
    m_Context->EndEncoding();
    
    // Unbound the framebuffer if necessary
    RendererAPI::EndRenderPass();
}

/**
 * @brief Clear the buffers to preset values.
 *
 * @param targets The rendering buffers to be cleared.
 */
void MetalRendererAPI::Clear(const RenderTargetBuffers& targets)
{
    @autoreleasepool
    {
        // Create a new Metal render pass descriptor
        MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        
        // Determine the active draw target override (attachment, cube face, mip level)
        auto drawTarget = m_ActiveFrameBuffer ? m_ActiveFrameBuffer->GetDrawTargetOverride() : FrameBufferDrawTarget{0};
        
        // Determine the number of color attachments to clear
        size_t colorAttachmentCount = drawTarget.IsAttachmentDefined() ? 1 : m_ActiveFrameBuffer->GetColorAttachments().size();
        
        // Lambda to retrieve the Metal texture for a given color attachment index
        auto getColorAttachment = [&](size_t i) -> id<MTLTexture>
        {
            if (!m_ActiveFrameBuffer)
                return reinterpret_cast<id<MTLTexture>>(m_Context->GetBackbufferTexture());

            uint32_t index = drawTarget.IsAttachmentDefined() ? drawTarget.AttachmentIndex : static_cast<uint32_t>(i);
            auto framebufferAttachment = std::dynamic_pointer_cast<MetalTexture>(m_ActiveFrameBuffer->GetColorAttachment(index));
            return reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
        };
        
        // Configure and clear all color attachments
        for (size_t i = 0; i < colorAttachmentCount; ++i)
        {
            auto attachment = getColorAttachment(i);
            
            descriptor.colorAttachments[i].clearColor = m_State->ClearColor;
            descriptor.colorAttachments[i].loadAction  = MTLLoadActionClear;
            descriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
            descriptor.colorAttachments[i].texture = attachment;
            
            // Apply cube map face/mip level override if defined
            if (drawTarget.IsCubeFaceDefined())
            {
                descriptor.colorAttachments[i].slice   = m_ActiveFrameBuffer->GetDrawTargetOverride().CubeFace;
                descriptor.colorAttachments[i].level   = m_ActiveFrameBuffer->GetDrawTargetOverride().MipLevel;
            }
        }
        
        // Configure depth attachment (if depth buffer is active)
        if (targets.Depth)
        {
            // Define the depth attachment to be used
            id<MTLTexture> attachment;
            if (!m_ActiveFrameBuffer)
            {
                // Create the depth texture for the screen target if it doesn't exist or needs to be updated
                CreateDepthTexture();
                attachment = reinterpret_cast<id<MTLTexture>>(m_State->RenderTarget.DepthAttachment);
            }
            else
            {
                // Get the depth attachment texture from the framebuffer
                std::shared_ptr<MetalTexture> framebufferAttachment =
                std::dynamic_pointer_cast<MetalTexture>(m_ActiveFrameBuffer->GetDepthAttachment());
                
                attachment = reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
            }
            
            descriptor.depthAttachment.clearDepth = 1.0;
            descriptor.depthAttachment.loadAction = MTLLoadActionClear;
            descriptor.depthAttachment.storeAction = m_ActiveFrameBuffer ? MTLStoreActionStore : MTLStoreActionDontCare;
            descriptor.depthAttachment.texture = attachment;
        }
        
        // Create command encoder and setup depth-stencil
        m_Context->InitCommandEncoder(descriptor, m_ActiveFrameBuffer ? "FB" : "SB");
        EnableDepthTesting(targets.Depth);
    } // autoreleasepool
}

/**
 * @brief Renders primitives from a drawable object using indexed drawing.
 *
 * @param drawable The drawable object containing the vertex and index buffers for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
 */
 void MetalRendererAPI::Draw(const std::shared_ptr<Drawable>& drawable,
                             const PrimitiveType &primitive)
{
    // Get the command encoder to encode rendering commands into the buffer
    auto encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());

     // Set the pipeline state
     auto metalDrawable = std::reinterpret_pointer_cast<MetalDrawable>(drawable);
     auto renderPipelineState = reinterpret_cast<id<MTLRenderPipelineState>>(
         metalDrawable->GetOrCreateRenderPipelineState(m_ActiveFrameBuffer));
     [encoder setRenderPipelineState:renderPipelineState];
     
    // Bind the drawable object
    drawable->Bind();
    // Draw primitives
    auto metalIndexBuffer = std::dynamic_pointer_cast<MetalIndexBuffer>(drawable->GetIndexBuffer());
    PIXEL_CORE_ASSERT(metalIndexBuffer, "Invalid buffer cast - not a Metal index buffer!");

    auto indexBuffer = reinterpret_cast<id<MTLBuffer>>(metalIndexBuffer->GetBuffer());
    [encoder
        drawIndexedPrimitives:utils::graphics::mtl::ToMetalPrimitive(primitive)
        indexCount:metalIndexBuffer->GetCount()
        indexType:MTLIndexTypeUInt32
        indexBuffer:indexBuffer
        indexBufferOffset:0
    ];
}

/**
 * @brief Creates and sets the depth attachment texture for the screen render target.
 */
void MetalRendererAPI::CreateDepthTexture()
{
    // Avoid unnecessary recreation
    glm::vec2 drawableSize = m_Context->GetBackbufferSize();
    
    if (m_State->RenderTarget.DepthAttachment &&
        m_State->RenderTarget.DepthAttachment.width == drawableSize.x &&
        m_State->RenderTarget.DepthAttachment.height == drawableSize.y)
    {
        return;
    }
    
    @autoreleasepool
    {
        // Get the Metal device from the context
        id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        
        // Define a depth texture
        MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
        
        // Define a depth texture
        descriptor.textureType = MTLTextureType2D;
        descriptor.pixelFormat = MTLPixelFormatDepth16Unorm;
        
        // Set the same texture dimensions
        descriptor.width = drawableSize.x;
        descriptor.height = drawableSize.y;
        
        // Set the texture's storage and usage modes
        descriptor.storageMode = MTLStorageModePrivate;
        descriptor.usage =  MTLTextureUsageShaderWrite | MTLTextureUsageRenderTarget;
        
        m_State->RenderTarget.DepthAttachment = [device newTextureWithDescriptor:descriptor];
        [descriptor release];
    } // autoreleasepool
}

/**
 * @brief Retrieves or creates a Metal depth state.
 * @return A pointer to the Metal depth state.
 */
void* MetalRendererAPI::GetOrCreateDepthState()
{
    @autoreleasepool
    {
        // Verify that the state has not been created yet
        auto it = m_State->Cache.Depth.find(m_State->DepthDescriptor);
        if (it != m_State->Cache.Depth.end())
            return reinterpret_cast<void*>(it->second);
        
        // Get the Metal device from the context
        auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        
        // Define the depth stencil descriptor and create the pipeline
        MTLDepthStencilDescriptor *descriptor = [[MTLDepthStencilDescriptor alloc] init];
        descriptor.depthWriteEnabled = m_State->DepthDescriptor.Enabled;
        descriptor.depthCompareFunction = utils::graphics::mtl::ToMetalCompareFunction(m_State->DepthDescriptor.Function);
        id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:descriptor];
        [descriptor release];
        
        // Cache the newly created state and return it
        m_State->Cache.Depth[m_State->DepthDescriptor] = depthState;
        return reinterpret_cast<void*>(depthState);
    } // autoreleasepool
}

} // namespace pixc
