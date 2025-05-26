#include "enginepch.h"
#include "Platform/Metal/MetalContext.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "Common/Renderer/Texture/Texture.h"
#include "Platform/Metal/Texture/MetalTexture.h"
#include "Platform/Metal/Buffer/MetalFrameBuffer.h"

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

/**
 * Holds core Metal objects related to the device and command queues.
 */
struct MetalDeviceResources
{
    ///< The Metal device (GPU) used for rendering.
    id<MTLDevice> Device;
    
    ///< The command queue for submitting rendering commands.
    id<MTLCommandQueue> RenderQueue;
    ///< The command queue for resource-related operations.
    id<MTLCommandQueue> ResourceQueue;
};

/**
 * Holds Metal objects related to the current rendering state.
 */
struct MetalRenderState
{
    ///< The current command buffer for rendering commands.
    id<MTLCommandBuffer> CommandBuffer;
    ///< The current render command encoder.
    id<MTLRenderCommandEncoder> Encoder;
    
    ///< The current render pipeline state.
    id<MTLRenderPipelineState> RenderPipelineState;
    ///< The current depth-stencil state.
    id<MTLDepthStencilState> DepthStencilState;
    
    ///< Viewport desccriptor.
    MTLViewport Viewport;
};

/**
 * Represents the rendering target resources for the screen.
 */
struct MetalScreenTarget
{
    ///< The Metal layer for presentation.
    CAMetalLayer* SwapChain;
    
    ///< The drawable surface for the screen.
    id<CAMetalDrawable> Drawable;
    
    ///< The color attachment texture (often from the Drawable).
    id<MTLTexture> ColorAttachment;
    ///< The depth attachment texture.
    id<MTLTexture> DepthAttachment;
};

/**
 * Holds the core Metal objects required for rendering.
 */
struct MetalContext::MetalState 
{
    ///< Resources associated with the Metal device.
    MetalDeviceResources DeviceResources;
    ///< Resources related to the current rendering state.
    MetalRenderState RenderState;
    ///< Rendering target resources for the screen.
    MetalScreenTarget ScreenTarget;
};

/**
 *  Constructs a Metal context for rendering.
 *
 *  @param windowHandle The GLFW window handle to associate with this context.
 */
MetalContext::MetalContext(GLFWwindow* windowHandle)
    : GraphicsContext(), m_WindowHandle(windowHandle)
{
    CORE_ASSERT(windowHandle, "Window handle is null!");
}

/**
 * Initializes the Metal rendering context.
 */
void MetalContext::Init() 
{
    // Initialize the Metal state object for managing Metal resources
    m_State = std::make_shared<MetalState>();

    // Initialize device resources
    InitializeMetalDeviceResources();
    // Initialize the screen rendering target
    InitializeScreenTarget();
    
    // Create the rendering states
    CreateDepthStencilState();
    // Clear the buffer for the next frame
    SetRenderTarget(glm::vec4(0.0f), RenderTargetBuffers());
    SwapBuffers();
}

/**
 * Get the default system Metal device (GPU).
 *
 * @return The metal device as a void pointer, or `nullptr` if the device was not found.
 */
void* MetalContext::GetDevice() const
{
    return reinterpret_cast<void*>(m_State->DeviceResources.Device);
}

/**
 * @brief Get the command queue for render-related operations.
 *
 * @return A pointer to the command queue, or `nullptr` if no command queue is available.
 */
void* MetalContext::GetRenderQueue() const
{
    return reinterpret_cast<void*>(m_State->DeviceResources.RenderQueue);
}

/**
 * @brief Get the command queue for resource-related operations.
 *
 * @return A pointer to the command queue, or `nullptr` if no command queue is available.
 */
void* MetalContext::GetResourceQueue() const
{
    return reinterpret_cast<void*>(m_State->DeviceResources.ResourceQueue);
}

/**
 * @brief Get the command buffer associated with the Metal context.
 *
 * @return A pointer to the command buffer, or `nullptr` if no command buffer is available.
 */
void* MetalContext::GetCommandBuffer() const
{
    return reinterpret_cast<void*>(m_State->RenderState.CommandBuffer);
}

/**
 * @brief Get the command encoder associated with the Metal context.
 *
 * @return A pointer to the command encoder, or `nullptr` if no command encoder is available.
 */
void* MetalContext::GetEncoder() const
{
    return reinterpret_cast<void*>(m_State->RenderState.Encoder);
}

/**
 *  Sets the window hints required for a Metal context.
 *
 *  This is a static function that should be called *before*
 *  creating the GLFW window to ensure a Metal-compatible
 *  context is created.
 */
void MetalContext::SetWindowHints()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

/**
 * Define if the window's buffer swap will be synchronized with the vertical
 * refresh rate of the monitor.
 *
 * @param enabled Enable or not the vertical synchronization.
 */
void MetalContext::SetVerticalSync(bool enabled)
{
    // Make sure the swap chain has been initialized
    if (!m_State->ScreenTarget.SwapChain) {
        CORE_WARN("MetalContext::SetVerticalSync() called before swap chain initialization!");
        return;
    }
    
    // Set the vertical synchronization
    BOOL objcEnabled = enabled ? YES : NO;
    [m_State->ScreenTarget.SwapChain setPresentsWithTransaction: !objcEnabled];
}

/**
 * Update metal layer size.
 */
void MetalContext::UpdateScreenbufferSize(unsigned int width, unsigned int height)
{
    // Ensure the swap chain is valid
    if (!m_State->ScreenTarget.SwapChain)
        return;
    
    // Update the Metal layer size to match the new window size
    m_State->ScreenTarget.SwapChain.drawableSize = CGSizeMake(width, height);
}

/**
 * Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void MetalContext::UpdateViewport(unsigned int x, unsigned int y,
                                  unsigned int width, unsigned int height)
{
    auto drawableSize = m_State->ScreenTarget.SwapChain.drawableSize;
    
    m_State->RenderState.Viewport.originX = static_cast<double>(x);
    m_State->RenderState.Viewport.originY = static_cast<double>(drawableSize.height - (y + height));
    m_State->RenderState.Viewport.width = static_cast<double>(width);
    m_State->RenderState.Viewport.height = static_cast<double>(height);
    
    m_State->RenderState.Viewport.znear = 0.0f;
    m_State->RenderState.Viewport.zfar = 1.0f;
    
    if (m_State->RenderState.Encoder)
        [m_State->RenderState.Encoder setViewport:m_State->RenderState.Viewport];
}

/**
 * Sets the active depth-stencil state for rendering.
 */
void MetalContext::EnableDepthStencilState()
{
    [m_State->RenderState.Encoder
        setDepthStencilState:m_State->RenderState.DepthStencilState];
}
/**
 * Sets the render target and configures the render pass descriptor.
 *
 * @param color The clear color to use for color attachments.
 * @param targets The render target buffers to clear (used for screen rendering).
 * @param framebuffer The `FrameBuffer` to render to (if not rendering to the screen).
 *                    If `nullptr`, rendering is targeted to the screen.
 */
void MetalContext::SetRenderTarget(const glm::vec4& color,
                                   const RenderTargetBuffers& targets,
                                   const std::shared_ptr<FrameBuffer>& framebuffer)
{
    // Determine the render target (screen or framebuffer)
    bool isScreenTarget = !framebuffer;
    // If rendering into the screen buffer, get the next drawable
    if (isScreenTarget)
    {
        m_State->ScreenTarget.Drawable = [m_State->ScreenTarget.SwapChain nextDrawable];
        m_State->ScreenTarget.ColorAttachment = m_State->ScreenTarget.Drawable.texture;
    }
    
    // Set the clear color
    MTLClearColor clearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
    
    // Create the render pass descriptor
    MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // Configure color attachment(s)
    size_t colorAttachmentCount = isScreenTarget ? 1 : framebuffer->GetColorAttachments().size();
    for (size_t i = 0; i < colorAttachmentCount; ++i)
    {
        // Define the color attachment to be used
        id<MTLTexture> attachment;
        if (isScreenTarget)
            attachment = m_State->ScreenTarget.ColorAttachment;
        else
        {
            // Get the color attachment texture from the framebuffer
            auto framebufferAttachment = std::dynamic_pointer_cast<MetalTexture>(
                 framebuffer->GetColorAttachment((unsigned int)i));
            
            attachment = reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
        }
        
        descriptor.colorAttachments[i].clearColor = clearColor;
        descriptor.colorAttachments[i].loadAction  = MTLLoadActionClear;
        descriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
        descriptor.colorAttachments[i].texture = attachment;
    }
    
    // Configure depth attachment (if depth buffer is active)
    if (targets.depthBufferActive)
    {
        // Create the depth texture for the screen target if it doesn't exist or needs to be updated
        CreateScreenDepthTexture();
        
        // Define the depth attachment to be used
        id<MTLTexture> attachment;
        if (isScreenTarget)
            attachment = m_State->ScreenTarget.DepthAttachment;
        else
        {
            // Get the depth attachment texture from the framebuffer
            std::shared_ptr<MetalTexture> framebufferAttachment =
                std::dynamic_pointer_cast<MetalTexture>(framebuffer->GetDepthAttachment());
            
            attachment = reinterpret_cast<id<MTLTexture>>(framebufferAttachment->MTLGetTexture());
        }
        
        descriptor.depthAttachment.clearDepth = 1.0;
        descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        descriptor.depthAttachment.storeAction = isScreenTarget ? MTLStoreActionDontCare : MTLStoreActionStore;
        
        descriptor.depthAttachment.texture = attachment;
    }
    
    // Create a new command buffer if necessary
    if (!m_State->RenderState.CommandBuffer)
        m_State->RenderState.CommandBuffer = [m_State->DeviceResources.RenderQueue commandBuffer];
    
    // Define a new command encoder
    m_State->RenderState.Encoder = [m_State->RenderState.CommandBuffer
                                        renderCommandEncoderWithDescriptor:descriptor];
    // Set the viewport information
    [m_State->RenderState.Encoder setViewport:m_State->RenderState.Viewport];
}

/**
 * Finalizes the current rendering pass by ending encoding.
 */
void MetalContext::EndEncoding()
{
    // Verify that the data has not been rendered yet
    if (!m_State->RenderState.Encoder)
        return;
    
    // End encoding in the command encoder
    [m_State->RenderState.Encoder endEncoding];
    m_State->RenderState.Encoder = nil;
}

/**
 *  Swaps the front and back buffers. This presents the rendered frame to the screen.
 */
void MetalContext::SwapBuffers()
{
    // finalize the encoding if necessary
    EndEncoding();
    
    if (!m_State->RenderState.CommandBuffer)
        return;
    
    // Present the drawable to the screen. This schedules the presentation
    // of the current framebuffer to the display
    [m_State->RenderState.CommandBuffer presentDrawable:m_State->ScreenTarget.Drawable];
    
    // Commit the command buffer (this submits all the commands to the GPU)
    [m_State->RenderState.CommandBuffer commit];
            
    // Nullify the command buffer after committing to prepare for the next frame
    m_State->RenderState.CommandBuffer = nil;
}

/**
 * Initializes core Metal device resources.
 */
void MetalContext::InitializeMetalDeviceResources()
{
    // Get the default system Metal device (GPU)
    m_State->DeviceResources.Device = MTLCreateSystemDefaultDevice();
    CORE_ASSERT(m_State->DeviceResources.Device, "Metal initialization failed: Could not get default Metal device!");

    // Create the command queue(s) for submitting rendering commands to the GPU
    m_State->DeviceResources.RenderQueue = [m_State->DeviceResources.Device newCommandQueue];
    m_State->DeviceResources.ResourceQueue = [m_State->DeviceResources.Device newCommandQueue];
    CORE_ASSERT(m_State->DeviceResources.RenderQueue && m_State->DeviceResources.ResourceQueue,
                "Metal initialization failed: Could not create command queue(s)!");
    
    // Display the Metal general information
    CORE_INFO("Using Metal:");
    NSString *deviceName = [m_State->DeviceResources.Device name];
    CORE_INFO("  Device: {0}", std::string([deviceName UTF8String]));
    
    // Create the command buffer and encoder
    m_State->RenderState.CommandBuffer = [m_State->DeviceResources.RenderQueue commandBuffer];
}

/**
 * Initializes the screen rendering target.
 */
void MetalContext::InitializeScreenTarget()
{
    // Create the Metal swap chain for presenting rendered frames to the screen
    m_State->ScreenTarget.SwapChain = [CAMetalLayer layer];
    // Associate the swap chain with the Metal device
    m_State->ScreenTarget.SwapChain.device = m_State->DeviceResources.Device;
    // Set the swap chain to opaque (no alpha blending)
    m_State->ScreenTarget.SwapChain.opaque = YES;

    // Get the NSWindow associated with the GLFW window handle for platform integration
    NSWindow* parentWindow = (NSWindow*)glfwGetCocoaWindow(m_WindowHandle);

    // Set up the swap chain as the layer for the window's content view
    parentWindow.contentView.layer = m_State->ScreenTarget.SwapChain;
    parentWindow.contentView.wantsLayer = YES;
    
    // Update the size of the screen buffer based on the windows size
    int width, height;
    glfwGetFramebufferSize(m_WindowHandle, &width, &height);
    UpdateScreenbufferSize(width, height);
    
    // Get the surface to output the render result in the screen
    m_State->ScreenTarget.Drawable = [m_State->ScreenTarget.SwapChain nextDrawable];
    m_State->ScreenTarget.ColorAttachment = m_State->ScreenTarget.Drawable.texture;
}

/**
 * Creates and sets the depth attachment texture for the screen render target.
 */
void MetalContext::CreateScreenDepthTexture()
{
    // Avoid unnecessary recreation
    auto drawableSize = m_State->ScreenTarget.SwapChain.drawableSize;
    if (m_State->ScreenTarget.DepthAttachment &&
        m_State->ScreenTarget.DepthAttachment.width == drawableSize.width &&
        m_State->ScreenTarget.DepthAttachment.height == drawableSize.height)
    {
        return;
    }
    
    // Define a depth texture
    MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
    
    // Define a depth texture
    descriptor.textureType = MTLTextureType2D;
    descriptor.pixelFormat = MTLPixelFormatDepth16Unorm;
    
    // Set the same texture dimensions
    descriptor.width = drawableSize.width;
    descriptor.height = drawableSize.height;
    
    // Set the texture's storage and usage modes
    descriptor.storageMode = MTLStorageModePrivate;
    descriptor.usage =  MTLTextureUsageShaderWrite | MTLTextureUsageRenderTarget;
    
    m_State->ScreenTarget.DepthAttachment = [m_State->DeviceResources.Device
                                                newTextureWithDescriptor:descriptor];
    [descriptor release];
}

/**
 * Creates the default depth-stencil state.
 */
void MetalContext::CreateDepthStencilState()
{
    MTLDepthStencilDescriptor *descriptor = [[MTLDepthStencilDescriptor alloc] init];
    descriptor.depthCompareFunction = MTLCompareFunctionLess;
    descriptor.depthWriteEnabled = YES;
    m_State->RenderState.DepthStencilState = [m_State->DeviceResources.Device
                                                newDepthStencilStateWithDescriptor:descriptor];
    [descriptor release];
}
