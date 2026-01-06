#include "pixcpch.h"
#include "Foundation/Renderer/Camera/PerspectiveCamera.h"

#include "Foundation/Input/Input.h"
#include "Foundation/Input/KeyCodes.h"
#include "Foundation/Input/MouseCodes.h"

#include "Foundation/Event/MouseEvent.h"
#include "Foundation/Event/KeyEvent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace pixc {

/**
 * @brief Generate a perspective camera.
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
    // Update camera matrices
    UpdateCameraMatrices();
}

/**
 * @brief Update the camera information in the current frame.
 *
 * @param ts Timestep.
 */
void PerspectiveCamera::OnUpdate(Timestep ts)
{
    // If no interaction is enabled, do not translate the camera
    if (!m_Enabled) return;
    
    // Calculate the distance of translation
    glm::vec3 distance = glm::vec3(0.0f);
    
    if(Input::IsKeyPressed(key::Q))     // up
        distance.y = ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::E))     // down
        distance.y = -ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::D))     // left
        distance.x = ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::A))     // right
        distance.x = -ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::W))     // front
        distance.z = ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::S))     // back
        distance.z = -ts * m_Movement.Translation;
    
    // Translate the camera
    Translate(distance);
}

/**
 * @brief Update the camera projection matrix (perspective projection).
 */
void PerspectiveCamera::UpdateProjectionMatrix()
{
    m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), GetAspectRatio(),
                                          m_NearPlane, m_FarPlane);
}

/**
 * @brief Calculate the distance between two points along a given direction.
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
 * @brief Translate the camera from one position to another.
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
 * @brief Rotate the camera in a static position.
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
 * @brief Orbit the camera around a target point.
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
 * @brief Zoom in/out the camera.
 *
 * @param delta The change in the field of view.
 */
void PerspectiveCamera::Zoom(const float delta)
{
    m_FieldOfView = std::clamp(m_FieldOfView - delta, 1.0f, 160.0f);
    UpdateProjectionMatrix();
}

} // namespace pixc
