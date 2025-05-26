#include "enginepch.h"
#include "Common/Input/Input.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Common/Core/Application.h"

using namespace pixc;

/**
 * Check if a specific keyboard key is currently pressed.
 *
 * @param key The key code of the keyboard key to check.
 *
 * @return `true` if the specified key is currently pressed, `false` otherwise.
*/
bool Input::IsKeyPressed(const KeyCode key)
{
    auto window = (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow();
    auto state = glfwGetKey(window, (int)key);
    
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

/**
 * Check if a specific mouse button is currently pressed.
 *
 * @param button The button code of the mouse button to check.
 * 
 * @return `true` if the specified mouse button is currently pressed, `false` otherwise.
 */
bool Input::IsMouseButtonPressed(const MouseCode button)
{
    auto window = (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow();
    auto state = glfwGetMouseButton(window, (int)button);
    
    return state == GLFW_PRESS;
}

/**
 * Get the current mouse position.
 *
 * @return A 2D vector representing the current mouse position.
 */
glm::vec2 Input::GetMousePosition()
{
    double x, y;
    
    auto window = (GLFWwindow*)Application::Get().GetWindow().GetNativeWindow();
    glfwGetCursorPos(window, &x, &y);
    
    return glm::vec2(x, y);
}
