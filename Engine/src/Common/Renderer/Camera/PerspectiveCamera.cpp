#include "enginepch.h"
#include "Common/Renderer/Camera/PerspectiveCamera.h"

#include "Common/Input/Input.h"
#include "Common/Input/KeyCodes.h"
#include "Common/Input/MouseCodes.h"

#include "Common/Event/MouseEvent.h"
#include "Common/Event/KeyEvent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace pixc;

/**
 * Generate a perspective camera.
 *
 * @param width Viewport size (width).
 * @param height Viewport size (height).
 * @param fov Field of view angle (degrees).
 * @param near Distance to the near plane.
 * @param far Distance to the far plane.
 */
PerspectiveCamera::PerspectiveCamera(const int width, const int height,
    const float fov, const float nearPlane, const float farPlane)
    : Camera(width, height, nearPlane, farPlane), m_FieldOfView(fov)
{
    UpdateCameraMatrices();
}

/**
 * Update the camera information in the current frame.
 *
 * @param ts Timestep.
 */
void PerspectiveCamera::OnUpdate(Timestep ts)
{
    static glm::vec2 initialMousePosition;
    
    // If no interaction is enabled, just update the mouse position
    if (!m_Enabled)
    {
        initialMousePosition = Input::GetMousePosition();
        return;
    }
    
    // Translation of the camera
    glm::vec3 distance = glm::vec3(0.0f);
    
    if(Input::IsKeyPressed(key::Q))     // up
        distance.y = ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::E))     // down
        distance.y = -ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::D))     // left
        distance.x = ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::A))     // right
        distance.x = -ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::W))     // front
        distance.z = ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::S))     // back
        distance.z = -ts * m_TranslationFactor;
    
    Translate(distance);
    
    // Camera rotation and orbit
    const glm::vec2 &mouse = Input::GetMousePosition();
    glm::vec2 deltaMouse = (mouse - initialMousePosition);
    deltaMouse *= ts;
    initialMousePosition = mouse;
    
    if (Input::IsMouseButtonPressed(mouse::ButtonLeft))
        Orbit(deltaMouse * m_OrbitFactor);
    if (Input::IsMouseButtonPressed(mouse::ButtonRight))
        Rotate(deltaMouse * m_RotationFactor);
}

/**
 * Handle an event on the application related to the camera.
 *
 * @param e Event to be handled.
 */
void PerspectiveCamera::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(
        BIND_EVENT_FN(PerspectiveCamera::OnMouseScroll));
    
}

/**
 * Update the camera projection matrix (perspective projection).
 */
void PerspectiveCamera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), GetAspectRatio(),
                                          m_NearPlane, m_FarPlane);
}

/**
 * Calculate the distance between two points along a given direction.
 *
 * @param p1 The first point.
 * @param p2 The second point.
 * @param direction The direction along which the distance is calculated.
 *
 * @return The calculated distance.
 */
glm::vec3 PerspectiveCamera::CalculateDistance(const glm::vec3& p1, const glm::vec3& p2,
                                               const glm::vec3& direction) const
{
    return p1 - direction * glm::length(p1 - p2);
}

/**
 * Zoom in/out the camera.
 *
 * @param delta The change in the field of view.
 */
void PerspectiveCamera::Zoom(const float delta)
{
    m_FieldOfView = std::clamp(m_FieldOfView - delta, 1.0f, 160.0f);
    UpdateProjectionMatrix();
}

/**
 * Translate the camera from one position to another.
 *
 * @param delta The distance to move the camera in x,y,z coordinates.
 */
void PerspectiveCamera::Translate(const glm::vec3 &delta)
{
    // Translate the camera
    m_Position += GetRightDirection() * delta.x;
    m_Position += GetUpDirection() * delta.y;
    m_Position += GetFowardDirection() * delta.z;
    
    // Update the camera's target
    m_Target = CalculateDistance(m_Position, m_Target, -GetFowardDirection());
    
    // Update the view matrix
    UpdateViewMatrix();
}

/**
 * Rotate the camera in a static position.
 *
 * @param delta The rotation angles in x and y axes.
 */
void PerspectiveCamera::Rotate(const glm::vec2 &delta)
{
    // Calculate the rotation angles
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_Rotation.x += yawSign * delta.y;
    m_Rotation.y += delta.x;
    
    // Update the camera's target
    m_Target = CalculateDistance(m_Position, m_Target, -GetFowardDirection());
    
    // Update the view matrix
    UpdateViewMatrix();
}

/**
 * Orbit the camera around a target point.
 *
 * @param delta The orbit rotation angles in x and y axes.
 */
void PerspectiveCamera::Orbit(const glm::vec2 &delta)
{
    // Calculate the orbit rotation angles
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_Rotation.x += yawSign * delta.y;
    m_Rotation.y += delta.x;

    // Limit the pitch to avoid flipping the camera
    m_Rotation.x = glm::clamp(m_Rotation.x, -89.0f, 89.0f);

    // Update the camera's position
    m_Position = CalculateDistance(m_Target, m_Position, GetFowardDirection());

    // Update the view matrix
    UpdateViewMatrix();
}

/**
 * Function to be called when a mouse scrolled event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool PerspectiveCamera::OnMouseScroll(MouseScrolledEvent &e)
{
    // Dolly the camera
    float delta = e.GetYOffset() * m_ZoomFactor;
    Zoom(delta);
    // Set the event as handled
    return true;
}
