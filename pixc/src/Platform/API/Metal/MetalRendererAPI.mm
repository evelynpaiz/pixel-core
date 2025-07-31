#include "pixcpch.h"
#include "Platform/Metal/MetalRendererAPI.h"

#include "Platform/Metal/Buffer/MetalIndexBuffer.h"
#include "Platform/Metal/Texture/MetalTexture.h"
#include "Platform/Metal/Shader/MetalShader.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"

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
    
    // Get the surface to output the render result if rendering to screen
    auto drawable = reinterpret_cast<id<CAMetalDrawable>>(m_Context->GetDrawable());
    m_State->RenderTarget.ColorAttachment = drawable.texture;
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
 void MetalRendererAPI::SetViewport(const unsigned int x, const unsigned int y,
                                    const unsigned int width, const unsigned int height)
 {
     // Get the size of the current drawable
     glm::vec2 drawableSize = m_Context->GetDrawableSize();
     
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
 * @brief Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.
 * @param function Depth function to be used for depth computation.
 */
void MetalRendererAPI::SetDepthTesting(const bool enabled,
                                       const DepthFunction function)
{
    m_State->DepthDescriptor.Enabled = enabled;
    
    if (function != DepthFunction::None)
    {
        m_State->DepthDescriptor.Function = function;
    }
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
    // If rendering into the screen buffer, get the next drawable
    if (!m_ActiveFramebuffer)
    {
        auto drawable = reinterpret_cast<id<CAMetalDrawable>>(m_Context->GetDrawable());
        m_State->RenderTarget.ColorAttachment = drawable.texture;
    }
    
    // Create the render pass descriptor
    MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // Configure color attachment(s)
    size_t colorAttachmentCount = m_ActiveFramebuffer ? m_ActiveFramebuffer->GetColorAttachments().size() : 1;
    for (size_t i = 0; i < colorAttachmentCount; ++i)
    {
        // Define the color attachment to be used
        id<MTLTexture> attachment;
        if (!m_ActiveFramebuffer)
            attachment = m_State->RenderTarget.ColorAttachment;
        else
        {
            // Get the color attachment texture from the framebuffer
            auto framebufferAttachment = std::dynamic_pointer_cast<MetalTexture>(
                 m_ActiveFramebuffer->GetColorAttachment((unsigned int)i));
            
            attachment = reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
        }
        
        descriptor.colorAttachments[i].clearColor = m_State->ClearColor;
        descriptor.colorAttachments[i].loadAction  = MTLLoadActionClear;
        descriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
        descriptor.colorAttachments[i].texture = attachment;
    }
    
    // Configure depth attachment (if depth buffer is active)
    if (targets.Depth)
    {
        // Create the depth texture for the screen target if it doesn't exist or needs to be updated
        CreateDepthTexture();
        
        // Define the depth attachment to be used
        id<MTLTexture> attachment;
        if (!m_ActiveFramebuffer)
            attachment = m_State->RenderTarget.DepthAttachment;
        else
        {
            // Get the depth attachment texture from the framebuffer
            std::shared_ptr<MetalTexture> framebufferAttachment =
                std::dynamic_pointer_cast<MetalTexture>(m_ActiveFramebuffer->GetDepthAttachment());
            
            attachment = reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
        }
        
        descriptor.depthAttachment.clearDepth = 1.0;
        descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        descriptor.depthAttachment.storeAction = m_ActiveFramebuffer ? MTLStoreActionStore : MTLStoreActionDontCare;
        descriptor.depthAttachment.texture = attachment;
    }
    
    // Define a new command encoder
    m_Context->InitCommandEncoder(descriptor, m_ActiveFramebuffer ? "FB" : "SB");
    
    // Defines a depth stencil state into the current command encoder
    SetDepthTesting(targets.Depth, DepthFunction::None);
    m_Context->SetDepthStencilState(GetOrCreateDepthState());
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
    id<MTLRenderCommandEncoder> encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());

     // Set the pipeline state
     auto metalDrawable = std::reinterpret_pointer_cast<MetalDrawable>(drawable);
     auto renderPipelineState = reinterpret_cast<id<MTLRenderPipelineState>>(metalDrawable->GetOrCreateRenderPipelineState(m_ActiveFramebuffer));
     [encoder setRenderPipelineState:renderPipelineState];
     
    // Bind the drawable object
    drawable->Bind();
    // Draw primitives
    auto metalIndexBuffer = std::dynamic_pointer_cast<MetalIndexBuffer>(drawable->GetIndexBuffer());
    PIXEL_CORE_ASSERT(metalIndexBuffer, "Invalid buffer cast - not a Metal index buffer!");

    id<MTLBuffer> indexBuffer = reinterpret_cast<id<MTLBuffer>>(metalIndexBuffer->GetBuffer());
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
    glm::vec2 drawableSize = m_Context->GetDrawableSize();
    
    if (m_State->RenderTarget.DepthAttachment &&
        m_State->RenderTarget.DepthAttachment.width == drawableSize.x &&
        m_State->RenderTarget.DepthAttachment.height == drawableSize.y)
    {
        return;
    }
    
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
}

/**
 * @brief Retrieves or creates a Metal depth state.
 * @return A pointer to the Metal depth state.
 */
void* MetalRendererAPI::GetOrCreateDepthState()
{
    // Verify that the state has not been created yet
    auto it = m_State->Cache.Depth.find(m_State->DepthDescriptor);
        if (it != m_State->Cache.Depth.end())
            return reinterpret_cast<void*>(it->second);
    
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
    
    // Define the depth stencil descriptor and create the pipeline
    MTLDepthStencilDescriptor *descriptor = [[MTLDepthStencilDescriptor alloc] init];
    descriptor.depthWriteEnabled = m_State->DepthDescriptor.Enabled;
    descriptor.depthCompareFunction = utils::graphics::mtl::ToMetalCompareFunction(m_State->DepthDescriptor.Function);
    id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:descriptor];
    [descriptor release];
    
    // Cache the newly created state and return it
    m_State->Cache.Depth[m_State->DepthDescriptor] = depthState;
    return reinterpret_cast<void*>(depthState);
}

} // namespace pixc
