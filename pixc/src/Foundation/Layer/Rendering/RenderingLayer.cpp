#include "Foundation/Layer/Rendering/RenderingLayer.h"

#include "Foundation/Renderer/RendererCommand.h"
#include "Foundation/Event/WindowEvent.h"

//#include "Common/Renderer/Shader/Shader.h"
//#include "Common/Renderer/Material/Material.h"

//#include <glm/gtc/matrix_transform.hpp>

namespace pixc {

/**
 * @brief Define a layer for a 3D viewer.
 */
RenderingLayer::RenderingLayer(int width, int height,
                               const std::string& name)
: Layer(name)
{}

/**
 * @brief Attach (add) the viewer layer to the rendering engine.
 */
void RenderingLayer::OnAttach()
{}

/**
 * @brief Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void RenderingLayer::OnUpdate(Timestep ts)
{
    RendererCommand::BeginRenderPass();
    
    RendererCommand::SetClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
    RendererCommand::Clear();
    
    RendererCommand::EndRenderPass();
}

/**
 * @brief Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void RenderingLayer::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
                                           BIND_EVENT_FN(RenderingLayer::OnWindowResize));
    
    // Handle the events on the camera
    //m_Camera->OnEvent(e);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool RenderingLayer::OnWindowResize(WindowResizeEvent &e)
{
    // Update the camera
    //m_Camera->SetViewportSize(e.GetWidth(), e.GetHeight());
    //m_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
    PIXEL_CORE_DEBUG("Window resize to {0} x {1}", e.GetWidth(), e.GetHeight());
    return true;
}

} // namespace pixc
