#include "pixcpch.h"
#include "Foundation/Renderer/Camera/OrthographicCamera.h"

#include "Foundation/Input/Input.h"
#include "Foundation/Input/KeyCodes.h"
#include "Foundation/Input/MouseCodes.h"

#include "Foundation/Event/MouseEvent.h"
#include "Foundation/Event/KeyEvent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace pixc {

/**
 * @brief Generate an orthographic camera.
 *
 * @param width Viewport size (width).
 * @param height Viewport size (height).
 * @param near Distance to the near plane.
 * @param far Distance to the far plane.
 */
OrthographicCamera::OrthographicCamera(const int width, const int height,
                                       const float nearPlane, const float farPlane)
: Camera(width, height, nearPlane, farPlane)
{
    // Change camera movement factor(s)
    SetTranslateFactor(2.0f);
    SetZoomFactor(0.05f);
    // Update camera matrices
    UpdateCameraMatrices();
}

/**
 * @brief Update the camera information in the current frame.
 *
 * @param ts Timestep.
 */
void OrthographicCamera::OnUpdate(Timestep ts)
{
    // If no interaction is enabled, do not translate the camera
    if (!m_Enabled) return;
    
    // Calculate the distance of translation
    glm::vec3 distance = glm::vec3(0.0f);
    
    if(Input::IsKeyPressed(key::W))     // up
        distance.y = ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::S))     // down
        distance.y = -ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::D))     // left
        distance.x = ts * m_Movement.Translation;
    if(Input::IsKeyPressed(key::A))     // right
        distance.x = -ts * m_Movement.Translation;
    
    // Translate the camera
    Translate(distance);
}

/**
 * @brief Update the camera view matrix.
 */
void OrthographicCamera::UpdateViewMatrix()
{
    // Compute the view matrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Position) *
                     glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
    m_ViewMatrix = glm::inverse(view);
}

/**
 * @brief Update the camera projection matrix (perspective projection).
 */
void OrthographicCamera::UpdateProjectionMatrix()
{
    // Calculate the aspect ratio
    float aspectRatio = (float)m_Width / (float)m_Height;
    
    // Update the projection matrix (left, right, bottom, top, near plane, far plane)
    m_ProjectionMatrix = glm::ortho(-aspectRatio * m_ZoomLevel, aspectRatio * m_ZoomLevel,
                                    -m_ZoomLevel, m_ZoomLevel, m_NearPlane, m_FarPlane);
}

/**
 * @brief Translate the camera from one position to another.
 *
 * @param delta The distance to move the camera in x,y,z coordinates.
 */
void OrthographicCamera::Translate(const glm::vec3 &delta)
{
    // Convert Z-axis rotation to radians
    float radians = glm::radians(m_Rotation.z);

    // Compute camera right vector in screen space
    glm::vec2 right = { cos(radians), sin(radians) };
    // Compute camera up vector in screen space
    glm::vec2 up = { -sin(radians), cos(radians) };

    // Translate the camera in its local (screen-aligned) space
    m_Position.x += right.x * delta.x + up.x * delta.y;
    m_Position.y += right.y * delta.x + up.y * delta.y;

    // Update the view matrix after movement
    UpdateViewMatrix();
}

/**
 * @brief Rotate the camera in a static position.
 *
 * @param delta The rotation angles in x and y axes.
 */
void OrthographicCamera::Rotate(const glm::vec2 &delta)
{
    // Calculate the rotation angle
    m_Rotation.z += delta.x;
    
    // Update the view matrix after the rotation
    UpdateViewMatrix();
}

/**
 * @brief Zoom in/out the camera.
 *
 * @param delta The change in the orthographic size.
 */
void OrthographicCamera::Zoom(const float delta)
{
    // Update the zoom level
    m_ZoomLevel = glm::clamp(m_ZoomLevel - delta, 0.01f, 10'000.0f);
    
    // Update the projection matrix after the zoom
    UpdateProjectionMatrix();
}

} // namespace pixc
