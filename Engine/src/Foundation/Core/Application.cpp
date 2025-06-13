#include "enginepch.h"
#include "Foundation/Core/Application.h"

#include "Foundation/Event/Event.h"
#include "Foundation/Event/WindowEvent.h"

#include "Foundation/Core/Timer.h"
#include "Foundation/Core/Timestep.h"

#include "Foundation/Renderer/Renderer.h"

namespace pixc {

// Define static variables
Application* Application::s_Instance = nullptr;

/**
 * @brief Generate a rendering application.
 *
 * @param name Application name.
 * @param width Size (width) of the application window.
 * @param height Size (height) of the application window.
 */
Application::Application(const std::string& name, const int width,
                         const int height)
{
    // Define the pointer to the application
    PIXEL_CORE_ASSERT(!s_Instance, "Application '{0}' already exists!", name);
    s_Instance = this;
    
    // Delay startup 1 sec to avoid window duplication (Xcode bug)
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Create the application window
    m_Window = std::make_unique<Window>(name, width, height);
    // Define the event callback function for the application
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    
    // Initialize the renderer
    Renderer::Init();
}

/**
 * @brief Add a new rendering layer to the application.
 *
 * @param layer New rendering layer.
 */
void Application::PushLayer(const std::shared_ptr<Layer>& layer)
{
    m_LayerStack.PushLayer(layer);
}

/**
 * @brief Add a new overlay layer (rendered on top) to the application.
 *
 * @param overlay New overlay layer.
 */
void Application::PushOverlay(const std::shared_ptr<Layer>& overlay)
{
    m_LayerStack.PushOverlay(overlay);
}

/**
 * @brief Remove a rendering layer from the application.
 *
 * @param layer Rendering layer.
 */
void Application::PopLayer(const std::shared_ptr<Layer>& layer)
{
    m_LayerStack.PopLayer(layer);
}

/**
 * @brief Remove an overlay layer (rendered on top) from the application.
 *
 * @param overlay Overlay layer.
 */
void Application::PopOverlay(const std::shared_ptr<Layer>& overlay)
{
    m_LayerStack.PopOverlay(overlay);
}

/**
 * @brief Run this current application.
 */
void Application::Run()
{
    
    static Timer timer;
    
    // Run until the user quits
    while (m_Running)
    {
        // Per-frame time logic
        Timestep deltaTime = (float)(timer.Elapsed());
        timer.Reset();
        
        // Render layers (from bottom to top)
        for (std::shared_ptr<Layer>& layer : m_LayerStack)
            layer->OnUpdate(deltaTime);
        
        // Update the window
        m_Window->OnUpdate();
    }
}

/**
 * @brief Callback function definition for event handling on the application.
 *
 * @param e Event to be handled.
 */
void Application::OnEvent(Event& e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
                                           BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<WindowCloseEvent>(
                                          BIND_EVENT_FN(Application::OnWindowClose));
    
    // Dispatch the event to the rendered layers (check from top to bottom)
    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
        if (e.Handled)
            break;
        (*it)->OnEvent(e);
    }
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool Application::OnWindowResize(WindowResizeEvent &e)
{
    m_Window->OnResize(e.GetWidth(), e.GetHeight());
    return false;
}

/**
 * @brief Function to be called when a window close event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool Application::OnWindowClose(WindowCloseEvent &e)
{
    
    // Close the application
    m_Running = false;
    PIXEL_CORE_INFO(e.GetDescription());
    
    return true;
}

} // namespace pixc
