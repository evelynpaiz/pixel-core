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
    UpdateCameraMatrices();
}

/**
 * @brief Update the camera information in the current frame.
 *
 * @param ts Timestep.
 */
void OrthographicCamera::OnUpdate(Timestep ts)
{
    static glm::vec2 initialMousePosition;
    
    // If no interaction is enabled, just update the mouse position
    if (!m_Enabled)
    {
        initialMousePosition = Input::GetMousePosition();
        return;
    }
    
    // Translation of the camera
    glm::vec2 distance = glm::vec2(0.0f);
    
    if(Input::IsKeyPressed(key::W))     // up
        distance.y = ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::S))     // down
        distance.y = -ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::D))     // left
        distance.x = ts * m_TranslationFactor;
    if(Input::IsKeyPressed(key::A))     // right
        distance.x = -ts * m_TranslationFactor;
    
    Translate(distance);
    
    // Camera rotation and orbit
    const glm::vec2 &mouse = Input::GetMousePosition();
    glm::vec2 deltaMouse = (mouse - initialMousePosition);
    deltaMouse *= ts;
    initialMousePosition = mouse;
    
    if (Input::IsMouseButtonPressed(mouse::ButtonLeft))
        Rotate(deltaMouse * m_RotationFactor);
}

/**
 * @brief Update the camera view matrix.
 */
void OrthographicCamera::UpdateViewMatrix()
{
    // Compute the view matrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Position)
    * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
    m_ViewMatrix = glm::inverse(view);
}

/**
 * @brief Update the camera projection matrix (perspective projection).
 */
void OrthographicCamera::UpdateProjectionMatrix()
{
    // Calculate the aspect ratio
    float aspectRatio = (float)m_Width / (float)m_Height;
    
    // Calculate the half width and height based on the orthographic size
    float halfOrthoWidth = m_OrthoSize * 0.5f * aspectRatio;
    float halfOrthoHeight = m_OrthoSize * 0.5f;
    
    // Calculate the boundaries for the orthographic projection
    float left = -halfOrthoWidth;
    float right = halfOrthoWidth;
    float bottom = -halfOrthoHeight;
    float top = halfOrthoHeight;
    
    // Update the projection matrix
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, m_NearPlane, m_FarPlane);
}

/**
 * @brief Translate the camera from one position to another.
 *
 * @param delta The distance to move the camera in x,y,z coordinates.
 */
void OrthographicCamera::Translate(const glm::vec2 &delta)
{
    // Translate the camera
    m_Position.x += delta.x;
    m_Position.y += delta.y;
    
    // Update the view matrix
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
    
    // Update the view matrix
    UpdateViewMatrix();
}

} // namespace pixc
