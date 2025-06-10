#pragma once

#include "Foundation/Input/KeyCodes.h"
#include "Foundation/Input/MouseCodes.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Static class for handling user input.
 *
 * The `Input` class provides static methods for checking and retrieving user input such as keyboard
 * and mouse events. It serves as a convenient interface to query input state without the need to
 * instantiate an object of this class.
 */
class Input
{
public:
    // Input checking
    // ----------------------------------------
    static bool IsKeyPressed(const KeyCode key);
    static bool IsMouseButtonPressed(const MouseCode button);
    
    // Getter(s)
    // ----------------------------------------
    static glm::vec2 GetMousePosition();
    /// @brief Get the current x-coordinate of the mouse position.
    /// @return The current x-coordinate of the mouse position.
    static float GetMouseX() { return GetMousePosition().x; }
    /// @brief Get the current y-coordinate of the mouse position.
    /// @return The current y-coordinate of the mouse position.
    static float GetMouseY() { return GetMousePosition().y; }
};

} // namespace pixc
