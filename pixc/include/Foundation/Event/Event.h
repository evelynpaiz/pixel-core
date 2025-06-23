#pragma once

#include <functional>
#include <sstream>
#include <spdlog/fmt/fmt.h>

#include "Foundation/Core/ClassUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Enumeration of supported event types.
 *
 * The `EventType` enumeration  provides a set of values to represent different types of events that
 * can occur. It is used to categorize and handle events in event-driven systems. The supported event
 * types include window events, keyboard events and mouse events.
 */
enum class EventType
{
    None = 0,
    WindowResize, WindowClose,
    KeyPressed, KeyReleased,
    MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
};

/**
 * @brief Enumeration of supported event categories.
 *
 * The `EventCategory` enumeration provides a set of values to represent different categories of events.
 * It is used to categorize and filter events in event-driven systems. Each event can belong to one or
 * more categories, allowing for efficient event handling and dispatching.
 *
 * The supported event categories include application events, input events, keyboard events, mouse events,
 * and mouse button events. Each category represents a specific aspect of event handling.
 */
enum EventCategory
{
    None = 0,
    EventCategoryApplication    = 1 << 0,
    EventCategoryInput          = 1 << 1,
    EventCategoryKeyboard       = 1 << 2,
    EventCategoryMouse          = 1 << 3,
    EventCategoryMouseButton    = 1 << 4
};

/**
 * @brief Represents an event on the application.
 *
 * The `Event` class serves as a base class for all types of events that can occur in the application.
 * It provides common functionality and interfaces to access information about the events.
 */
class Event {
    public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the event.
    virtual ~Event() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the type of the event.
    /// @return An `EventType`.
    virtual EventType GetEventType() const = 0;
    /// @brief Get the categories of the event.
    /// @return The`EventCategory` flags.
    virtual int GetCategoryFlags() const = 0;
    /// @brief Get the name of the event.
    /// @return The event name.
    virtual const char* GetName() const = 0;
    /// @brief Get the description of the event.
    /// @return The event description.
    virtual std::string GetDescription() const { return GetName(); }
    /// @brief Check if the event is of certain category
    bool IsInCategory(const EventCategory category) const
    {
        return GetCategoryFlags() & category;
    }
    
    // Event variables
    // ----------------------------------------
    public:
    ///< Status of the event.
    bool Handled = false;
    
    // Disable its construction, if it is not done from a child
    // ----------------------------------------
    protected:
    Event() = default;
    Event(const Event &) = default;
    Event(Event &&) = default;
    
    Event &operator=(const Event &) = default;
    Event &operator=(Event &&) = default;
};

/**
 * @brief Macro to override the definition of the event types inside a child event class.
 *
 * @param type The name of the event type to be defined.
 */
#define EVENT_CLASS_TYPE(type)\
static EventType GetEventTypeStatic() { return EventType::type; }\
virtual EventType GetEventType() const override { return GetEventTypeStatic(); }\
virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)\
virtual int GetCategoryFlags() const override { return category; }

/**
 * @brief Represents a dispatcher (of events) inside the application.
 *
 * The `EventDispatcher` class provides a mechanism to dispatch events. It is responsible for verifying
 * the event type and invoking the appropriate callback function if the event matches.
 *
 * Copying or moving `EventDispatcher` objects is disabled to ensure single ownership and prevent
 * unintended duplication.
 */
class EventDispatcher {
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a dispatcher for the event.
    EventDispatcher(Event& event)  : m_Event(event) {}
    /// @brief Delete the event dispatcher.
    ~EventDispatcher() = default;
    
    // Dispatcher
    // ----------------------------------------
    /// @brief Dispatch an event to the appropiate callback function.
    /// @tparam E Event type.
    /// @param func Callback function to handle the event.
    /// @return `true` if the event was successfully dispatched and handled.
    template <typename E>
    bool Dispatch(const std::function<bool(E&)> &func)
    {
        // Verify event type
        if (m_Event.GetEventType() == E::GetEventTypeStatic())
        {
            m_Event.Handled |= func(*(E*)&m_Event);
            return true;
        }
        
        // The event is not the same type
        return false;
    }
    
    // Event dispatcher variables
    // ----------------------------------------
    private:
    ///< Event (reference) to be dispatched.
    Event &m_Event;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(EventDispatcher);
};

/// Binding event function definition.
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

} // namespace pixc
