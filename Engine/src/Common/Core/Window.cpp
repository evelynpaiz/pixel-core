#include "enginepch.h"
#include "Common/Core/Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Common/Event/WindowEvent.h"
#include "Common/Event/KeyEvent.h"
#include "Common/Event/MouseEvent.h"

namespace pixc {

// --------------------------------------------
// Variable initialization
// --------------------------------------------

/// Window counter for GLFW.
static unsigned int g_WindowCount = 0;

// --------------------------------------------
// Event management
// --------------------------------------------

/**
 * @brief Function to be called when a GLFW error occurs.
 *
 * @param error Error type.
 * @param description Description of the error.
 */
static void ErrorCallback(const int error, const char *description) noexcept
{
    CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param window Native window.
 * @param width Updated window size (width).
 * @param height Updated window size (height).
 */
static void WindowResizeCallback(GLFWwindow *window, int width, int height) noexcept
{
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    
    // Update the size of the window
    data.Width = static_cast<unsigned int>(width);
    data.Height = static_cast<unsigned int>(height);
    
    // Call the event callback function with a window resize event
    if (data.EventCallback)
    {
        WindowResizeEvent event(data.Title, width, height);
        data.EventCallback(event);
    }
}

/**
 * @brief Function to be called when a window close event happens.
 *
 * @param window Native window.
 */
static void WindowCloseCallback(GLFWwindow *window)
{
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    
    // Call the event callback function with a window close event
    if (data.EventCallback)
    {
        WindowCloseEvent event(data.Title);
        data.EventCallback(event);
    }
}

/**
 * @brief Function to be called when a key event happens.
 *
 * @param window Native window.
 * @param key The key thas has been pressed or released.
 * @param scancode The system specific scan code of the key.
 * @param action (pressed, released or repeat).
 * @param mods Bit fild describing the modifiers keys held down.
 */
static void KeyCallback(GLFWwindow *window, int key, int scancode,
                        int action, int mods) noexcept
{
    // Key pressed counter
    static unsigned int keyCount = 1;
    
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    if (!data.EventCallback)
        return;
    
    // Call the event callback function with the respective keyboard event
    switch (action)
    {
        case GLFW_PRESS:
        {
            KeyPressedEvent event(key, keyCount);
            data.EventCallback(event);
            break;
        }
        case GLFW_RELEASE:
        {
            keyCount = 1;
            KeyReleasedEvent event(key);
            data.EventCallback(event);
            break;
        }
        case GLFW_REPEAT:
        {
            keyCount++;
            KeyPressedEvent event(key, keyCount);
            data.EventCallback(event);
            break;
        }
    }
}

/**
 * @brief Function to be called when a mouse button event happens.
 *
 * @param window Native window.
 * @param button The button thas has been pressed or released.
 * @param action (pressed, released or scroll).
 * @param mods Bit fild describing the modifiers keys held down.
 */
static void MouseButtonCallback(GLFWwindow *window, int button, int action,
                                int mods) noexcept
{
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    if (!data.EventCallback)
        return;
    
    // Call the event callback function with the respective mouse event
    switch (action)
    {
        case GLFW_PRESS:
        {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
        }
        case GLFW_RELEASE:
        {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            break;
        }
    }
}

/**
 * @brief Function to be called when a mouse scrolled event happens.
 *
 * @param window Native window.
 * @param xOffset The scroll offset in the x-axis (horizontally).
 * @param yOffset The scroll offset in the y-axis (vertically).
 */
static void MouseScrolledCallback(GLFWwindow *window, double xOffset,
                                  double yOffset) noexcept
{
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    if (!data.EventCallback)
        return;
    
    MouseScrolledEvent event((float)xOffset, (float)yOffset);
    data.EventCallback(event);
}

/**
 * @brief Function to be called when a mouse moved event happens.
 *
 * @param window Native window.
 * @param x The mouse position in the x-axis.
 * @param y The mouse position in the y-axis.
 */
static void MouseMovedCallback(GLFWwindow *window, double x, double y) noexcept
{
    // Recover the window information
    WindowData &data = *(WindowData*)glfwGetWindowUserPointer(window);
    if (!data.EventCallback)
        return;
    
    MouseMovedEvent event((float)x, (float)y);
    data.EventCallback(event);
}

// --------------------------------------------
// Window
// --------------------------------------------
/**
 * @brief Generate a window.
 *
 * @param title Window name.
 * @param width Size (width) of the window.
 * @param height Size (height) of the window.
 */
Window::Window(const std::string& title, const unsigned int width,
               const unsigned int height)
: m_Data(title, width, height)
{
    Init();
}

/**
 * @brief Delete the window.
 */
Window::~Window()
{
    Shutdown();
}

/**
 * @brief Update the window.
 */
void Window::OnUpdate() const
{
    // Swap front and back buffers
    m_Context->SwapBuffers();
    
    // Poll for and process events
    glfwPollEvents();
}

/**
 * @brief Update the size information when the window is resized.
 */
void Window::OnResize(const unsigned int width, const unsigned int height) const
{
    m_Context->UpdateScreenbufferSize(width, height);
}

/**
 * @brief Define if the window's buffer swap will be synchronized with the vertical
 * refresh rate of the monitor.
 *
 * @param enabled Enable or not the vertical synchronization.
 */
void Window::SetVerticalSync(bool enabled)
{
    m_Context->SetVerticalSync(enabled);
    m_Data.VerticalSync = enabled;
}

/**
 *  @brief Initialize the window with all its corresponding components.
 */
void Window::Init()
{
    // Check if GLFW is already initialized, if not initialize it
    if (g_WindowCount == 0)
    {
        CORE_TRACE("Initializing GLFW");
        
        int success = glfwInit();
        CORE_ASSERT(success, "Failed to initialize GLFW!");
        
        glfwSetErrorCallback(ErrorCallback);
    }
    
    // Define the window hints for on the graphics context
    GraphicsContext::SetWindowHints();
    
    // Create a windowed mode window and its OpenGL context
    m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height,
                                m_Data.Title.c_str(), nullptr, nullptr);
    CORE_ASSERT(m_Window, "Failed to create a GLFW window!");
    ++g_WindowCount;
    
    // Initialize the rendering context
    m_Context = GraphicsContext::Create(m_Window);
    m_Context->Init();
    
    // Set a vertical synchronization
    SetVerticalSync(true);
    
    // Set the pointer to the window data
    glfwSetWindowUserPointer(m_Window, &m_Data);
    
    // Define the event callbacks
    glfwSetFramebufferSizeCallback(m_Window, WindowResizeCallback);
    glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
    
    glfwSetKeyCallback(m_Window, KeyCallback);
    
    glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
    glfwSetScrollCallback(m_Window, MouseScrolledCallback);
    glfwSetCursorPosCallback(m_Window, MouseMovedCallback);
    
    // Retrieve the size of the window in pixel size and update the window information
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    
    m_Data.Width = static_cast<unsigned int>(width);
    m_Data.Height = static_cast<unsigned int>(height);
    
    m_Context->UpdateScreenbufferSize(m_Data.Width, m_Data.Height);
    
    // Show window created message
    CORE_INFO("Creating '{0}' window ({1} x {2})", m_Data.Title,
              m_Data.Width, m_Data.Height);
}

/**
 * @brief Shutdown and close the window.
 */
void Window::Shutdown()
{
    // Close the window
    glfwDestroyWindow(m_Window);
    g_WindowCount -= 1;
    
    // Check if is the only window available, if so, terminate GLFW too
    if (g_WindowCount == 0)
    {
        CORE_TRACE("Terminating GLFW");
        glfwTerminate();
    }
}

} // namespace pixc
