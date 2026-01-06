#pragma once

#include <glm/glm.hpp>

#include "Foundation/Renderer/Camera/Camera.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a perspective camera that captures the scene and displays it in a viewport.
 *
 * The `PerspectiveCamera`  class is a derived class of the `Camera` class, providing a perspective
 * projection for rendering. It inherits all the properties and functionality of the base Camera class and
 * adds additional methods to manipulate the camera's orientation and field of view. The camera's view
 * matrix and projection matrix are automatically updated when camera properties change.
 *
 * Copying or moving `PerspectiveCamera` objects is disabled to ensure single ownership and
 * prevent unintended camera duplication.
 */
class PerspectiveCamera : public Camera
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    PerspectiveCamera(const int width, const int height, const float fov = 45.0f,
                      const float nearPlane = 0.1f, const float farPlane = 100.0f);
    
    // Rendering
    // ----------------------------------------
    void OnUpdate(Timestep ts) override;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the camera rotation angle in the x-axis.
    /// @return Pitch angle (degrees).
    float GetPitch() const { return m_Rotation.x; }
    /// @brief Get the camera rotation angle in the y-axis.
    /// @return Yaw angle (degrees).
    float GetYaw() const { return m_Rotation.y; }
    /// @brief Get the camera rotation angle in the z-axis.
    /// @return Roll angle (degrees).
    float GetRoll() const { return m_Rotation.z; }
    /// @brief Get the camera field of view.
    /// @return Field of view angle (degrees).
    float GetFieldOfView() const { return m_FieldOfView; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Change the camera rotation in the x-axis.
    /// @param pitch The camera rotation angle.
    void SetPitch(const float pitch)
    {
        m_Rotation.x = pitch;
        UpdateViewMatrix();
    }
    /// @brief Change the camera rotation in the y-axis.
    /// @param yaw The camera rotation angle.
    void SetYaw(const float yaw)
    {
        m_Rotation.y = yaw;
        UpdateViewMatrix();
    }
    /// @brief Change the camera rotation in the z-axis.
    /// @param roll The camera rotation angle.
    void SetRoll(const float roll)
    {
        m_Rotation.z = roll;
        UpdateViewMatrix();
    }
    /// @brief Change the camera field of view.
    /// @param fov The camera field of view (angle).
    void SetFieldOfView(const float fov)
    {
        m_FieldOfView = fov;
        UpdateProjectionMatrix();
    }
    
protected:
    // Transformation matrices
    // ----------------------------------------
    void UpdateProjectionMatrix() override;
    
    // Getter(s)
    // ----------------------------------------
    glm::vec3 CalculateDistance(const glm::vec3& p1, const glm::vec3& p2,
                                const glm::vec3& direction) const;
    
    // Camera movements
    // ----------------------------------------
    void Translate(const glm::vec3 &delta) override;
    void Rotate(const glm::vec2 &delta) override;
    void Orbit(const glm::vec2 &delta) override;
    void Zoom(const float delta) override;
    
    // Perspective camera variables
    // ----------------------------------------
protected:
    ///< Camera field of view (angle in degrees).
    float m_FieldOfView;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PerspectiveCamera);
};

} // namespace pixc
