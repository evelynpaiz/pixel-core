#include "pixcpch.h"
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
 * @brief Get the current texture of the screen's backbuffer drawable.
 *
 * @return A pointer to the Metal texture (id<MTLTexture>), or `nullptr` if no drawable is currently available.
 */
void* MetalContext::GetBackbufferTexture() const
{
    if (!m_State->SwapChain.Drawable)
        return nullptr;
        
    return reinterpret_cast<void*>(m_State->SwapChain.Drawable.texture);
}

/**
 * Retrieves the current backbuffer size.
 *
 * @return A vector with the width and height of the backbuffer.
 */
glm::vec2 MetalContext::GetBackbufferSize() const
{
    auto size = m_State->SwapChain.Layer.drawableSize;
    return glm::vec2(size.width, size.height);
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
    if (!m_State->SwapChain.Layer)
    {
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
    
    // If there's no command buffer, there's nothing to commit or present
    if (!m_State->Frame.CommandBuffer)
        return;
    
    @autoreleasepool
    {
        // Present the current drawable (framebuffer) to the screen if defined
        if (m_State->SwapChain.Drawable)
        {
            [m_State->Frame.CommandBuffer presentDrawable:m_State->SwapChain.Drawable];
            [m_State->SwapChain.Drawable release];
            m_State->SwapChain.Drawable = nil;
        }
        
        // Commit and wait for completion to ensure GPU finishes rendering this frame
        [m_State->Frame.CommandBuffer commit];
        [m_State->Frame.CommandBuffer waitUntilCompleted];
        
        // Release the command buffer now that it's no longer needed
        [m_State->Frame.CommandBuffer release];
        m_State->Frame.CommandBuffer = nil;
        
        m_State->SwapChain.Drawable = [[m_State->SwapChain.Layer nextDrawable] retain];
    } // autoreleasepool
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
    UpdateBufferSize(width, height);
}

/**
 * Initializes the command buffer if necessary.
 */
bool MetalContext::InitCommandBuffer()
{
    if (!m_State->Frame.CommandBuffer)
    {
        @autoreleasepool
        {
            // Create a command buffer and retain it to persist outside the autorelease scope
            m_State->Frame.CommandBuffer = [[m_State->Device.RenderQueue commandBuffer] retain];
        } // autoreleasepool
        // Label for debugging/profiling in Xcode
        m_State->Frame.CommandBuffer.label = @"Main";
        return true;
    }
    return false;
}

/**
 * @brief Initialize a command encoder for a rendering pass using a descriptor.
 *
 * @param descriptor Render pass descriptor.
 */
void MetalContext::InitCommandEncoder(const void *descriptor, const std::string& name)
{
    PIXEL_CORE_ASSERT(!m_State->Frame.Encoder, "Render encoder still active! — call EndEncoding first");
    
    @autoreleasepool
    {
        // Create the encoder and manually retain it to extend lifetime
        MTLRenderPassDescriptor* passDescriptor = (__bridge MTLRenderPassDescriptor*)descriptor;
        id<MTLRenderCommandEncoder> encoder = [m_State->Frame.CommandBuffer renderCommandEncoderWithDescriptor:passDescriptor];
        m_State->Frame.Encoder = [encoder retain];
    } // autoreleasepool
    
    // Label for debugging/profiling in Xcode
    if (!name.empty())
        m_State->Frame.Encoder.label = [NSString stringWithUTF8String:name.c_str()];
}

/**
 * @brief Finalizes the current rendering pass by ending encoding.
 */
void MetalContext::EndEncoding()
{
    // Verify that the data has not been rendered yet
    if (!m_State->Frame.Encoder)
        return;

    // End encoding in the command encoder
    [m_State->Frame.Encoder endEncoding];
    
    // Release the retained encoder
    [m_State->Frame.Encoder release];
    m_State->Frame.Encoder = nil;
}

/**
 * Set the size of the drawable screen buffer.
 *
 * @param width The width of the buffer.
 * @param height The height of the buffer.
 */
void MetalContext::UpdateBufferSize(const unsigned int width,
                                    const unsigned int height)
{
    // TODO: to be checked! (maybe the viewport needs to be updated too)
    m_State->SwapChain.Layer.drawableSize = CGSizeMake(width, height);
}

/**
 * @brief Set the viewport for rendering.
 *
 * @param descriptor Pointer to the metal viewport descriptor.
 */
void MetalContext::UpdateViewport(const void* descriptor)
{
    // Get the viewport descriptor
    auto viewport = reinterpret_cast<const MTLViewport *>(descriptor);
    
    // Update the encoder if necessary
    if (m_State->Frame.Encoder)
        [m_State->Frame.Encoder setViewport:*viewport];
}

/**
 * @brief Sets the active depth-stencil state for rendering.
 *
 * @param state Pointer to the depth stencil state defined. 
 */
void MetalContext::SetDepthStencilState(const void* state)
{
    // Verify that the encoder has been defined
    if (!m_State->Frame.Encoder)
        return;
    
    // Set the depth stencil state
    auto depthStencilState = reinterpret_cast<id<MTLDepthStencilState>>(state);
    [m_State->Frame.Encoder setDepthStencilState:depthStencilState];
}

} // namespace pixc
