#pragma once

#include "Foundation/Core/ClassUtils.h"

#include "Foundation/Event/Event.h"
#include "Foundation/Renderer/GraphicsContext.h"

struct GLFWwindow;

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents the information inside a window.
 */
struct WindowData
{
    // Window data variables
    // ----------------------------------------
    ///< Window title.
    std::string Title;
    ///< Window size.
    uint32_t Width, Height;
    ///< Vertical synchronization with the monitor.
    bool VerticalSync;
    
    ///< Callback function to handle events.
    std::function<void(Event&)> EventCallback;
    
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Define the information of a window.
    /// @param title Window name.
    /// @param width Size (width) of the window.
    /// @param height Size (height) of the window.
    WindowData(const std::string& title, const uint32_t width,
               const uint32_t height, bool verticalSync = true)
    : Title(title), Width(width), Height(height), VerticalSync(verticalSync)
    {}
    /// @brief delete the data of the window.
    ~WindowData() = default;
};

/**
 * Defines a window that contains the running application.
 *
 * The `Window` class represents a window in the application. It provides methods to create, update,
 * and interact with the window. The event callback function can be set to handle window events.
 *
 * Copying or moving `Window` objects is disabled to ensure single ownership and prevent unintended
 * window duplication.
 */
class Window
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    Window(const std::string& title, const uint32_t width, const uint32_t height);
    ~Window();
    
    // Handler(s)
    // ----------------------------------------
    void OnUpdate() const;
    void OnResize(const uint32_t width, const uint32_t height) const;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the title of the window.
    /// @return The window name.
    std::string GetTitle() const { return m_Data.Title; }
    /// @brief Get the size (width) of the window.
    /// @return The width of the window.
    int GetWidth() const { return m_Data.Width; }
    /// @brief Get the size (height) of the window.
    /// @return The height of the window.
    int GetHeight() const { return m_Data.Height; }
    /// @brief Check if there is a vertical synchronization with the monitor.
    /// @return `true` if the window is synchronized.
    bool IsVerticalSync() const { return m_Data.VerticalSync; }
    /// @brief Get the GLFW window.
    /// @return The native window.
    void* GetNativeWindow() const { return m_Window; }
    
    // Setter(s)
    // ----------------------------------------
    void SetVerticalSync(bool enabled);
    /// @brief Set the event callback function for this window.
    /// @param callback The event callback function.
    void SetEventCallback(const std::function<void(Event&)>& callback)
    {
        m_Data.EventCallback = callback;
    }
    
private:
    // Init/Close
    // ----------------------------------------
    void Init();
    void Shutdown();
    
    // Window variables
    // ----------------------------------------
private:
    ///< Native window (GLFW).
    GLFWwindow* m_Window;
    ///< Graphics context for rendering.
    std::unique_ptr<GraphicsContext> m_Context;
    
    ///< Window information.
    WindowData m_Data;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Window);
};

} // namespace pixc
