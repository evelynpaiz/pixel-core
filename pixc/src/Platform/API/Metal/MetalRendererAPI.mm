#include "pixcpch.h"
#include "Platform/Metal/MetalRendererAPI.h"

#include "Platform/Metal/Buffer/MetalIndexBuffer.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"

#include "Platform/Metal/MetalRendererUtils.h"

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
    std::unordered_map<DepthDescriptor, id<MTLDepthStencilState>> Depth;
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
    ///< Depth configuration.
    DepthDescriptor DepthState;
    
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
    m_State->DepthState.Enabled = enabled;
    
    if (function != DepthFunction::None)
    {
        m_State->DepthState.Function = function;
    }
}

/**
 * @brief Initialize a new rendering pass.
 */
void MetalRendererAPI::BeginRenderPass()
{
    m_Context->InitCommandBuffer();
}

/**
 * @brief Finalize the current rendering pass.
 */
void MetalRendererAPI::EndRenderPass()
{
    m_Context->EndEncoding();
}

/**
 * @brief Clear the buffers to preset values.
 *
 * @param targets The rendering buffers to be cleared.
 */
void MetalRendererAPI::Clear(const RenderTargetBuffers& targets)
{
    // Create the render pass descriptor
    MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // Configure color attachment
    auto drawable = reinterpret_cast<id<CAMetalDrawable>>(m_Context->GetDrawable());
    
    descriptor.colorAttachments[0].clearColor = m_State->ClearColor;
    descriptor.colorAttachments[0].loadAction  = MTLLoadActionClear;
    descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    descriptor.colorAttachments[0].texture = drawable.texture;
    
    // Configure depth attachment (if depth buffer is active)
    if (targets.depthBufferActive)
    {
        // Create the depth texture for the screen target if it doesn't exist or needs to be updated
        CreateDepthTexture();
        
        descriptor.depthAttachment.clearDepth = 1.0;
        descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        descriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
        
        descriptor.depthAttachment.texture = m_State->RenderTarget.DepthAttachment;
    }
    
    // Define a new command encoder
    m_Context->InitCommandEncoder(descriptor, "Scene");
    
    // Defines a depth stencil state into the current command encoder
    SetDepthTesting(targets.depthBufferActive, DepthFunction::None);
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
 * @brief Creates a default depth-stencil state.
 */
void* MetalRendererAPI::GetOrCreateDepthState()
{
    // Verify that the state has not been created yet
    auto it = m_State->Cache.Depth.find(m_State->DepthState);
        if (it != m_State->Cache.Depth.end())
            return reinterpret_cast<void*>(it->second);
    
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
    
    // Define the depth stencil descriptor and create the pipeline
    MTLDepthStencilDescriptor *descriptor = [[MTLDepthStencilDescriptor alloc] init];
    descriptor.depthWriteEnabled = m_State->DepthState.Enabled;
    descriptor.depthCompareFunction = utils::graphics::mtl::ToMetalCompareFunction(m_State->DepthState.Function);
    id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:descriptor];
    [descriptor release];
    
    // Cache the newly created state and return it
    m_State->Cache.Depth[m_State->DepthState] = depthState;
    return reinterpret_cast<void*>(depthState);
}

/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param targets Active rendering targets.
 
 void MetalRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets)
 {
 SetRenderTarget(glm::vec4(glm::vec4(0.0f)), targets);
 }
 */

/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param color Background color.
 * @param targets Active rendering targets.
 
 void MetalRendererAPI::SetRenderTarget(const glm::vec4& color,
 const RenderTargetBuffers& targets)
 {
 m_Context->SetRenderTarget(color, targets);
 SetDepthTesting(targets.depthBufferActive);
 }
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param framebuffer Framebuffer whose targets should be activated.
 
 void MetalRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets,
 const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 SetRenderTarget(glm::vec4(glm::vec4(0.0f)), targets, framebuffer);
 }
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param color Background color.
 * @param framebuffer Framebuffer whose targets should be activated and cleared.
 
 void MetalRendererAPI::SetRenderTarget(const glm::vec4& color,
 const RenderTargetBuffers& targets,
 const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 framebuffer->Bind();
 m_Context->SetRenderTarget(color, targets, framebuffer);
 SetDepthTesting(targets.depthBufferActive);
 }
 */
/**
 * Finalize the current rendering pass.
 
 void MetalRendererAPI::EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 m_Context->EndEncoding();
 RendererAPI::EndRenderPass(framebuffer);
 }
 */

} // namespace pixc
