#pragma once

#include "Foundation/Event/Event.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a window event on the application.
 *
 * The `WindowEvent` class is a base class for window events. It inherits from the `Event` class
 * and provides common functionality and attributes related to window events.
 */
class WindowEvent : public Event
{
    public:
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the title of the window.
    /// @return The window name.
    std::string GetTitle() const { return m_Title; }
    
    // Define the different categories of the event
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
    
    protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a window event.
    /// @param title The window title.
    WindowEvent(const std::string& title)
    : m_Title(title)
    {}
    
    // Window event variables
    // ----------------------------------------
    protected:
    ///< Code of the key pressed.
    std::string m_Title;
};

/**
 * @brief An event representing the resizing of the application window.
 *
 * The `WindowResizeEvent` class is derived from `WindowEvent` and represents the event
 * of the application window being resized. It provides additional functionality and attributes specific
 * to window resize events.
 */
class WindowResizeEvent : public WindowEvent
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a window resize event.
    /// @param title The window title.
    /// @param width Updated window size (width).
    /// @param height Updated window size (height).
    WindowResizeEvent(const std::string& title, uint32_t width,
                      uint32_t height)
    : WindowEvent(title), m_Width(width), m_Height(height)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the resize event size.
    /// @return The updated size (width).
    uint32_t GetWidth() const { return m_Width; }
    /// @brief Get the resize event size.
    /// @return The updated size (height).
    uint32_t GetHeight() const { return m_Height; }
    /// @brief Get the description of the event.
    /// @return Event description (updated size).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Window '" << m_Title << "' resized: " << m_Width << " x " << m_Height;
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(WindowResize)
    
    // Window resize event variables
    // ----------------------------------------
    private:
    ///< Size.
    uint32_t m_Width, m_Height;
};

/**
 * @brief An event representing the closing of the application window.
 *
 * The `WindowCloseEvent` class is derived from `WindowEvent` and represents the event of
 * the application window being closed. It provides additional functionality and attributes specific
 * to window close events.
 */
class WindowCloseEvent : public WindowEvent {
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a window close event.
    /// @param title The window title.
    WindowCloseEvent(const std::string& title)
    : WindowEvent(title)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the description of the event.
    /// @return Event description (closed window).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Closing '" << m_Title << "' window";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(WindowClose)
};

} // namespace pixcs
