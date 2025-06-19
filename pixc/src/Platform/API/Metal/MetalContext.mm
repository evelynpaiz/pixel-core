#include "enginepch.h"
#include "Platform/Metal/MetalContext.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

namespace pixc {

/**
 * Holds core Metal objects related to the device and command queues.
 */
struct MetalDeviceContext
{
    ///< The Metal device (GPU) used for rendering.
    id<MTLDevice> Gpu;
    
    ///< The command queue for submitting rendering commands.
    id<MTLCommandQueue> RenderQueue;
    ///< The command queue for resource-related operations.
    id<MTLCommandQueue> ResourceQueue;
};

/**
 * Holds per-frame rendering state such as the command buffer and encoder.
 */
struct MetalFrameState
{
    ///< The current command buffer for rendering commands.
    id<MTLCommandBuffer> CommandBuffer;
    ///< The current render command encoder.
    id<MTLRenderCommandEncoder> Encoder;
};

/**
 * Represents the rendering target resources for the screen.
 */
struct MetalSwapChainTarget
{
    ///< The Metal layer for presentation.
    CAMetalLayer* Layer;
    ///< The drawable surface for the screen.
    id<CAMetalDrawable> Drawable;
};

/**
 * Holds the core Metal objects required for rendering.
 */
struct MetalContext::MetalState
{
    ///< Resources associated with the Metal device.
    MetalDeviceContext Device;
    ///< Per-frame rendering state.
        MetalFrameState Frame;
    ///< Rendering target resources for the screen.
    MetalSwapChainTarget SwapChain;
};

/**
 *  Constructs a Metal context for rendering.
 *
 *  @param windowHandle The GLFW window handle to associate with this context.
 */
MetalContext::MetalContext(GLFWwindow* windowHandle)
: GraphicsContext(), m_WindowHandle(windowHandle)
{
    PIXEL_CORE_ASSERT(windowHandle, "Window handle is null!");
}

/**
 * Initializes the Metal rendering context.
 */
void MetalContext::Init()
{
    // Initialize the Metal state object for managing Metal resources
    m_State = std::make_shared<MetalState>();
    
    // Initialize the device and the swap chain
    InitDevice();
    InitSwapChain();
}

/**
 * Get the default system Metal device (GPU).
 *
 * @return The metal device as a void pointer, or `nullptr` if the device was not found.
 */
void* MetalContext::GetDevice() const
{
    return reinterpret_cast<void*>(m_State->Device.Gpu);
}

/**
 * @brief Get the command queue for render-related operations.
 *
 * @return A pointer to the command queue, or `nullptr` if no command queue is available.
 */
void* MetalContext::GetRenderQueue() const
{
    return reinterpret_cast<void*>(m_State->Device.RenderQueue);
}

/**
 * @brief Get the command queue for resource-related operations.
 *
 * @return A pointer to the command queue, or `nullptr` if no command queue is available.
 */
void* MetalContext::GetResourceQueue() const
{
    return reinterpret_cast<void*>(m_State->Device.ResourceQueue);
}

/**
 * @brief Get the current Metal command buffer used for rendering.
 *
 * @return A pointer to the command buffer, or `nullptr` if no command buffer is currently active.
 */
void* MetalContext::GetCommandBuffer() const
{
    return reinterpret_cast<void*>(m_State->Frame.CommandBuffer);
}

/**
 * @brief Get the current Metal render command encoder.
 *
 * @return A pointer to the render command encoder, or `nullptr` if no encoder is currently active.
 */
void* MetalContext::GetCommandEncoder() const
{
    return reinterpret_cast<void*>(m_State->Frame.Encoder);
}

/**
 * Retrieves the native texture of the current drawable.
 *
 * @return A pointer to the drawable texture, or `nullptr` if no command queue is available.
 */
void* MetalContext::GetDrawable() const
{
    return reinterpret_cast<void*>(m_State->SwapChain.Drawable);
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
    if (!m_State->SwapChain.Layer) {
        PIXEL_CORE_WARN("MetalContext::SetVerticalSync() called before swap chain initialization!");
        return;
    }
    
    // Set the vertical synchronization
    BOOL objcEnabled = enabled ? YES : NO;
    [m_State->SwapChain.Layer setPresentsWithTransaction: !objcEnabled];
}

/**
 *  Swaps the front and back buffers. This presents the rendered frame to the screen.
 */
void MetalContext::SwapBuffers()
{
    // Finalize the encoding if necessary
    EndEncoding();
    
    if (!m_State->Frame.CommandBuffer)
        return;
    
    // Present the drawable to the screen. This schedules the presentation
    // of the current framebuffer to the display
    [m_State->Frame.CommandBuffer presentDrawable:m_State->SwapChain.Drawable];
    
    // Commit the command buffer (this submits all the commands to the GPU)
    [m_State->Frame.CommandBuffer commit];
    
    // Nullify the command buffer after committing to prepare for the next frame
    m_State->Frame.CommandBuffer = nil;
    
    // Get the next drawable to be used
    m_State->SwapChain.Drawable = [m_State->SwapChain.Layer nextDrawable];
}

/**
 * Initializes core Metal device resources.
 */
void MetalContext::InitDevice()
{
    // Get the default system Metal device (GPU)
    m_State->Device.Gpu = MTLCreateSystemDefaultDevice();
    PIXEL_CORE_ASSERT(m_State->Device.Gpu, "Metal initialization failed: Could not get default Metal device!");
    
    // Create the command queue(s) for submitting rendering commands to the GPU
    m_State->Device.RenderQueue = [m_State->Device.Gpu newCommandQueue];
    m_State->Device.ResourceQueue = [m_State->Device.Gpu newCommandQueue];
    PIXEL_CORE_ASSERT(m_State->Device.RenderQueue && m_State->Device.ResourceQueue,
                "Metal initialization failed: Could not create command queue(s)!");
    
    // Display the Metal general information
    PIXEL_CORE_INFO("Using Metal:");
    NSString *deviceName = [m_State->Device.Gpu name];
    PIXEL_CORE_INFO("  Device: {0}", std::string([deviceName UTF8String]));
}

/**
 * Initializes the screen rendering target.
 */
void MetalContext::InitSwapChain()
{
    // Create the Metal swap chain for presenting rendered frames to the screen
    m_State->SwapChain.Layer = [CAMetalLayer layer];
    // Associate the swap chain with the Metal device
    m_State->SwapChain.Layer.device = m_State->Device.Gpu;
    // Set the swap chain to opaque (no alpha blending)
    m_State->SwapChain.Layer.opaque = YES;
    
    // Get the NSWindow associated with the GLFW window handle for platform integration
    NSWindow* parentWindow = (NSWindow*)glfwGetCocoaWindow(m_WindowHandle);
    
    // Set up the swap chain as the layer for the window's content view
    parentWindow.contentView.layer = m_State->SwapChain.Layer;
    parentWindow.contentView.wantsLayer = YES;
    
    // Update the size of the screen buffer based on the windows size
    int width, height;
    glfwGetFramebufferSize(m_WindowHandle, &width, &height);
    // TODO: verify
    UpdateScreenbufferSize(width, height);
    
    // Get the surface to output the render result in the screen
    m_State->SwapChain.Drawable = [m_State->SwapChain.Layer nextDrawable];
}

/**
 * Initializes the command buffer if necessary.
 */
bool MetalContext::InitCommandBuffer()
{
    if (!m_State->Frame.CommandBuffer)
    {
        m_State->Frame.CommandBuffer = [m_State->Device.RenderQueue commandBuffer];
        m_State->Frame.CommandBuffer.label = @"Main";
        return true;
    }
    return false;
}

/**
 * Initialize a command encoder for a rendering pass using a descriptor.
 *
 * @param descriptor Render pass descriptor.
 */
void MetalContext::InitCommandEncoder(const void *descriptor, const std::string& name)
{
    PIXEL_CORE_ASSERT(!m_State->Frame.Encoder, "Render encoder still active! — call EndEncoding first");
    
    // Create the command encoder
    MTLRenderPassDescriptor* passDescriptor = (__bridge MTLRenderPassDescriptor*)descriptor;
    m_State->Frame.Encoder = [m_State->Frame.CommandBuffer renderCommandEncoderWithDescriptor:passDescriptor];
    
    // Set debug label if provided
    if (!name.empty())
        m_State->Frame.Encoder.label = [NSString stringWithUTF8String:name.c_str()];
}

/**
 * Finalizes the current rendering pass by ending encoding.
 */
void MetalContext::EndEncoding()
{
    // Verify that the data has not been rendered yet
    if (!m_State->Frame.Encoder)
        return;

    // End encoding in the command encoder
    [m_State->Frame.Encoder endEncoding];
    m_State->Frame.Encoder = nil;
}

} // namespace pixc
