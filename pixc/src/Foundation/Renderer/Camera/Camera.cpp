#include "pixcpch.h"
#include "Foundation/Renderer/Camera/Camera.h"

#include "Foundation/Input/Input.h"

#include "Foundation/Event/MouseEvent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace pixc {

/**
 * @brief Handle an event on the application related to the camera.
 *
 * @param e Event to be handled.
 */
void Camera::OnEvent(Event &e)
{
    // If no interaction is enabled, do not handle the event
    if (!m_Enabled) return;
    
    // Dispatch the mouse events
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Camera::OnMouseMove));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Camera::OnMouseScroll));
}

/**
 * @brief Get the camera orientation defined as a quaternion.
 *
 * @return The camera orientation.
 */
glm::quat Camera::GetOrientation() const
{
    return glm::quat(glm::radians(glm::vec3(-m_Rotation.x, -m_Rotation.y,
                                            -m_Rotation.z)));
}

/**
 * @brief Get the camera's up direction vector.
 *
 * @return The up vector.
 */
glm::vec3 Camera::GetUpDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

/**
 * @brief Get the camera's right direction vector.
 *
 * @return The right vector.
 */
glm::vec3 Camera::GetRightDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

/**
 * @brief Get the camera's forward direction vector.
 *
 * @return The forward vector.
 */
glm::vec3 Camera::GetFowardDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

/**
 * @brief Change the camera resolution.
 *
 * @param width Viewport size (width).
 * @param height Viewport size (height).
 */
void Camera::SetViewportSize(const int width, const int height)
{
    // Check input values
    if (width <= 0 || height <= 0)
    {
        PIXEL_CORE_WARN("Attempted to rezize camera resolution to {0}, {1}", width, height);
        return;
    }
    
    // Define the new resolution value
    m_Width = width;
    m_Height = height;
    
    // Update the projetion matrix
    UpdateProjectionMatrix();
}

/**
 * @brief Calculate the pitch angle of the camera (x-axis).
 *
 * @return The pitch angle in degrees.
 */
float Camera::CalculatePitch() const
{
    glm::vec3 direction = glm::normalize(m_Target - m_Position);
    return glm::degrees(asin(-direction.y));
}

/**
 * @brief Calculate the yaw angle of the camera (y-axis).
 *
 * @return The yaw angle in degrees.
 */
float Camera::CalculateYaw() const
{
    glm::vec3 direction = glm::normalize(m_Target - m_Position);
    return glm::degrees(atan2(direction.x, -direction.z));
}

/**
 * @brief Update the camera view matrix.
 */
void Camera::UpdateViewMatrix()
{
    // Get the camera current orientation
    glm::quat orientation = GetOrientation();
    // Compute the view matrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Position)
    * glm::toMat4(orientation);
    m_ViewMatrix = glm::inverse(view);
}

/**
 * @brief Update the camera projection matrix (perspective projection).
 */
void Camera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::mat4(1.0f);
}

/**
 * @brief Update the camera matrices.
 */
void Camera::UpdateCameraMatrices()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

/**
 * @brief Function to be called when a mouse movement event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool Camera::OnMouseMove(MouseMovedEvent &e)
{
    // Saves the previous mouse position
    static glm::vec2 initialMousePosition;
    
    // Get the mouse position and calculate the delta movement
    const glm::vec2 &mouse = Input::GetMousePosition();
    glm::vec2 deltaMouse = (mouse - initialMousePosition);
    initialMousePosition = mouse;
    
    // Update camera orbit and rotation
    if (Input::IsMouseButtonPressed(mouse::ButtonLeft))
        Orbit(deltaMouse * m_Movement.Orbit);
    if (Input::IsMouseButtonPressed(mouse::ButtonRight))
        Rotate(deltaMouse * m_Movement.Rotation);
    
    // Set the event as handled
    return true;
}

/**
 * @brief Function to be called when a mouse scrolled event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool Camera::OnMouseScroll(MouseScrolledEvent &e)
{
    // Dolly the camera
    float delta = e.GetYOffset() * m_Movement.Zoom;
    Zoom(delta);
    // Set the event as handled
    return true;
}

} // namespace pixc
