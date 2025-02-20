#include "enginepch.h"
#include "Common/Core/Application.h"

#include "Common/Event/Event.h"
#include "Common/Event/WindowEvent.h"

#include "Common/Core/Timer.h"
#include "Common/Core/Timestep.h"

#include "Common/Renderer/Renderer.h"

// Define static variables
Application* Application::s_Instance = nullptr;

/**
 * Generate a rendering application.
 *
 * @param name Application name.
 * @param width Size (width) of the application window.
 * @param height Size (height) of the application window.
 */
Application::Application(const std::string& name, const int width,
                         const int height)
{
    // Define the pointer to the application
    CORE_ASSERT(!s_Instance, "Application '{0}' already exists!", name);
    s_Instance = this;
    
    // Create the application window
    m_Window = std::make_unique<Window>(name, width, height);
    // Define the event callback function for the application
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
    
    // Initialize the renderer
    Renderer::Init();
}

/**
 * Add a new rendering layer to the application.
 *
 * @param layer New rendering layer.
 */
void Application::PushLayer(const std::shared_ptr<Layer>& layer)
{
    m_LayerStack.PushLayer(layer);
}

/**
 * Add a new overlay layer (rendered on top) to the application.
 *
 * @param overlay New overlay layer.
 */
void Application::PushOverlay(const std::shared_ptr<Layer>& overlay)
{
    m_LayerStack.PushOverlay(overlay);
}

/**
 * Remove a rendering layer from the application.
 *
 * @param layer Rendering layer.
 */
void Application::PopLayer(const std::shared_ptr<Layer>& layer)
{
    m_LayerStack.PopLayer(layer);
}

/**
 * Remove an overlay layer (rendered on top) from the application.
 * 
 * @param overlay Overlay layer.
 */
void Application::PopOverlay(const std::shared_ptr<Layer>& overlay)
{
    m_LayerStack.PopOverlay(overlay);
}

/**
 * Run this current application.
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
 * Callback function definition for event handling on the application.
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
 * Function to be called when a window resize event happens.
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
 * Function to be called when a window close event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool Application::OnWindowClose(WindowCloseEvent &e)
{
    // Close the application
    m_Running = false;
    CORE_INFO(e);
    
    return true;
}
