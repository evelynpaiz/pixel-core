#include "pixcpch.h"
#include "Foundation/Renderer/RendererAPI.h"

#include "Foundation/Renderer/GraphicsContext.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#ifdef __APPLE__
#include "Platform/Metal/MetalRendererAPI.h"
#endif

namespace pixc {

// Define static variables
RendererAPI::API RendererAPI::s_API = RendererAPI::API::Metal;

/**
 * Creates a new Renderer API instance based on the selected API.
 *
 * @return A unique pointer to the created RendererAPI instance.
 */
std::unique_ptr<RendererAPI> RendererAPI::Create()
{
    switch (s_API)
    {
        case RendererAPI::API::None:
            PIXEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
            
        case RendererAPI::API::OpenGL:
            return std::make_unique<OpenGLRendererAPI>();
            
#ifdef __APPLE__
        case RendererAPI::API::Metal:
            return std::make_unique<MetalRendererAPI>();
#endif
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

/**
 * @brief Initialize a new rendering pass.
 *
 * @param framebuffer Buffer to hold to result of the rendered pass.
 */
void RendererAPI::BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    // If no framebuffer is active, return and render into the screen buffer
    if (!framebuffer)
        return;
    
    // Update the information of the currently bound framebuffer
    m_ActiveFrameBuffer = framebuffer;
    m_ActiveFrameBuffer->Bind();                // render into the framebuffer
}

/**
 * @brief Finalize the current rendering pass.
 */
void RendererAPI::EndRenderPass()
{
    // If no framebuffer is active, return
    if (!m_ActiveFrameBuffer)
        return;
    
    // Unbound the framebuffer if necessary
    m_ActiveFrameBuffer->Unbind();
    m_ActiveFrameBuffer = nullptr;
}

/**
 * @brief Clear the buffers to preset values.
 */
void RendererAPI::Clear()
{
    RenderTargetMask clearTargets = m_ActiveFrameBuffer
            ? m_ActiveFrameBuffer->GetEnabledTargets()          // Clear buffers in the active framebuffer
            : RenderTargetMask::Color;                          // Clear color only for screen
    
    Clear(clearTargets);
}

} // namespace pixc
