#pragma once

#include "Common/Core/Window.h"
#include "Common/Layer/LayerStack.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class WindowResizeEvent;
class WindowCloseEvent;

/**
 * @brief Represents the main rendering application.
 *
 * The `Application` class represents the main application handler for the rendering engine. It
 * provides methods to create and run the application, as well as handle window events. Layers can
 * be pushed and popped to manage the rendering process. The class encapsulates a window and
 * manages the application's main loop.
 *
 * Copying or moving `Application` objects is disabled to ensure single ownership and prevent
 * unintended duplication.
 */
class Application
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    Application(const std::string &name = "Pixel Core Renderer", const int width = 800,
                const int height = 600);
    /// @brief Delete the application.
    virtual ~Application() = default;
    
    // Run
    // ----------------------------------------
    void Run();
    
    // Events handler(s)
    // ----------------------------------------
    void OnEvent(Event& e);
    
    // Rendering layers
    // ----------------------------------------
    void PushLayer(const std::shared_ptr<Layer>& layer);
    void PushOverlay(const std::shared_ptr<Layer>& overlay);
    void PopLayer(const std::shared_ptr<Layer>& layer);
    void PopOverlay(const std::shared_ptr<Layer>& overlay);
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the application instance.
    /// @return This application.
    static Application& Get() { return *s_Instance; }
    Window& GetWindow() { return *m_Window; }
    
private:
    // Events handler(s)
    // ----------------------------------------
    bool OnWindowResize(WindowResizeEvent &e);
    bool OnWindowClose(WindowCloseEvent &e);
    
    // Application variables
    // ----------------------------------------
private:
    ///< Application window.
    std::unique_ptr<Window> m_Window;
    ///< Application status.
    bool m_Running = true;
    
    ///< Layers to be rendered.
    LayerStack m_LayerStack;
    
private:
    ///< Pointer to this application.
    static Application* s_Instance;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Application);
};

} // namespace pixc
