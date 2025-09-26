#pragma once

#include "Foundation/Event/Event.h"
#include "Foundation/Input/KeyCodes.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a keyboard event on the application.
 *
 * The `KeyEvent` class is a base class for keyboard events. It inherits from the `Event` class
 * and provides common functionality and attributes related to keyboard events.
 */
class KeyEvent : public Event
{
    public:
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the numerical code of the key.
    /// @return The key code.
    KeyCode GetKeyCode() const { return m_KeyCode; }
    
    // Define the different categories of the event
    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    
    protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a keyboard event.
    /// @param keyCode Numerical value of the key.
    KeyEvent(const KeyCode keyCode)
        : m_KeyCode(keyCode)
    {}
    
    // Key event variables
    // ----------------------------------------
    protected:
    ///< Code of the key pressed/released.
    KeyCode m_KeyCode;
};

/**
 * @brief An event representing the pressing of a key on the keyboard.
 *
 * The `KeyPressedEvent` class is derived from `KeyEvent` and represents the event of a key being
 * pressed on the keyboard. It provides additional functionality and attributes specific to key pressed events.
 */
class KeyPressedEvent : public KeyEvent
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a key pressed event.
    /// @param keyCode Numerical value of the key.
    /// @param repeatCount Number of times the key has been pressed.
    KeyPressedEvent(const KeyCode keyCode, const uint32_t repeatCount)
        : KeyEvent(keyCode), m_RepeatCount(repeatCount)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the number of times the key has been pressed.
    /// @return The key repeat count.
    uint32_t GetRepeatCount() const { return m_RepeatCount; }
    /// @brief Get the description of the event.
    /// @return Event description (key currently pressed).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Key '" << m_KeyCode << "' pressed " << m_RepeatCount << " times";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(KeyPressed)
    
    // Key pressed event variables
    // ----------------------------------------
    private:
    ///< Key pressed counter.
    uint32_t m_RepeatCount;
};

/**
 * @brief An event representing the releasing of a key on the keyboard.
 *
 * The `KeyReleasedEvent` class is derived from `KeyEvent` and represents the event of a key
 * being released on the keyboard. It provides additional functionality and attributes specific to key
 * released events.
 */
class KeyReleasedEvent : public KeyEvent
{
    public:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a key released event.
    /// @param keyCode Numerical value of the key.
    KeyReleasedEvent(const KeyCode keyCode)
        : KeyEvent(keyCode)
    {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the description of the event.
    /// @return Event description (key currently released).
    std::string GetDescription() const override
    {
        std::stringstream ss;
        ss << "Key '" << m_KeyCode << "' released";
        return ss.str();
    }
    
    // Define the getter methods for the event type using the macro
    EVENT_CLASS_TYPE(KeyReleased)
};

} // namespace pixc
