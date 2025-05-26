#pragma once

#include "Common/Event/Event.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
//namespace pixc {
using namespace pixc;

/**
 * @brief Represents a mouse button event on the application.
 *
 * The `MouseButtonEvent` class is a base class for mouse button events. It inherits from the
 * `Event` class and provides common functionality and attributes related to mouse button events.
 */
class MouseButtonEvent : public Event
{
    public:
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the numerical code of the mouse button.
    /// @return The mouse code.
    int GetMouseButton() const { return m_Button; }
    
    // Define the different categories of the event
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    
    protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a mouse button event.
    /// @param button Numerical value of the button.
    MouseButtonEvent(const int button)
    : m_Button(button)
    {}
    
    // Mouse button event variables
    // ----------------------------------------
    protected:
    ///< Code of the button pressed/released.
    int m_Button;
};

/**
 * @brief An event representing the pressing of a button on the mouse.
 *
 * The `MouseButtonPressedEvent` class is derived from `MouseButtonEvent` and represents
 * the event of a mouse button being pressed. It provides additional functionality and attributes specific
 * to mouse button pressed events.
 */
class MouseButtonPressedEvent : public MouseButtonEvent
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a mouse button pressed event.
    /// @param button Numerical value of the button.
    MouseButtonPressedEvent(const int button)
    : MouseButtonEvent(button)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the description of the event.
    /// @return Event description (mouse button currently pressed).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Mouse button '" << m_Button << "' pressed";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(MouseButtonPressed)
};

/**
 * @brief An event representing the releasing of a button on the mouse.
 *
 * The `MouseButtonReleasedEvent` class is derived from `MouseButtonEvent` and represents
 * the event of a mouse button being released. It provides additional functionality and attributes specific
 * to mouse button released events.
 */
class MouseButtonReleasedEvent : public MouseButtonEvent
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a mouse button released event.
    /// @param button Numerical value of the button.
    MouseButtonReleasedEvent(const int button)
    : MouseButtonEvent(button)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the description of the event.
    /// @return Event description (mouse button currently released).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Mouse button '" << m_Button << "' released";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(MouseButtonReleased)
};

/**
 * @brief An event representing the scrolling of the mouse.
 *
 * The `MouseScrolledEvent` class is derived from `Event` and represents the event of the
 * mouse scrolling. It provides functionality and attributes specific to mouse scrolling events.
 */
class MouseScrolledEvent : public Event
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a mouse scrolled event.
    /// @param xOffset The scroll offset in the x-axis (horizontally).
    /// @param yOffset The scroll offset in the y-axis (vertically).
    MouseScrolledEvent(const float xOffset, const float yOffset)
    : m_XOffset(xOffset), m_YOffset(yOffset)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the scroll offset.
    /// @return The offset in the x-axis.
    float GetXOffset() const { return m_XOffset; }
    /// @brief Get the scroll offset.
    /// @return The offset in the y-axis.
    float GetYOffset() const { return m_YOffset; }
    /// @brief Get the description of the event.
    /// @return Event description (mouse being currently scrolled).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Mouse scrolled (" << m_XOffset << " , " << m_YOffset << ")";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(MouseScrolled)
    
    // Define the different categories of the event
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
    // Mouse scrolled event variables
    // ----------------------------------------
    private:
    ///< Scroll offsets.
    float m_XOffset, m_YOffset;
};

/**
 * @brief An event representing the movement of the mouse.
 *
 * The `MouseMovedEvent` class is derived from `Event` and represents the event of the
 * mouse moving. It provides functionality and attributes specific to mouse movement events.
 */
class MouseMovedEvent : public Event
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a mouse moved event.
    /// @param x The mouse position in the x-axis.
    /// @param y The mouse position in the y-axis.
    MouseMovedEvent(const float x, const float y)
    : m_X(x), m_Y(y)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the mouse position.
    /// @return The mouse position in the x-axis.
    float GetX() const { return m_X; }
    /// @brief Get the mouse position.
    /// @return The mouse position in the y-axis.
    float GetY() const { return m_Y; }
    /// @brief Get the description of the event.
    /// @return Event description (mouse being currently moved).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Mouse moved (" << m_X << " , " << m_Y << ")";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(MouseMoved)
    
    // Define the different categories of the event
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    
    // Mouse moved event variables
    // ----------------------------------------
    private:
    ///< Mouse position.
    float m_X, m_Y;
};

//} // namespace pixc
