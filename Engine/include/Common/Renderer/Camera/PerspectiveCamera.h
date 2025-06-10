#pragma once

#include <glm/glm.hpp>

#include "Common/Renderer/Camera/Camera.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class MouseScrolledEvent;

/**
 * Represents a perspective camera that captures the scene and displays it in a viewport.
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
    
    // Event handler
    // ----------------------------------------
    void OnEvent(Event &e) override;
    
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
    
    /// @brief Update the zooming in/out scaling factor.
    /// @param v Scaling factor value.
    void SetZoomFactor(const float v) { m_ZoomFactor = v; }
    /// @brief Update the camera translation scaling factor.
    /// @param v Scaling factor value.
    void SetTranslateFactor(const float v) { m_TranslationFactor = v; }
    /// @brief Update the camera rotation scaling factor.
    /// @param v Scaling factor value.
    void SetRotateFactor(const float v) { m_RotationFactor = v; }
    /// @brief Update the camera oribitin scaling factor.
    /// @param v Scaling factor value.
    void SetOrbitFactor(const float v) { m_ZoomFactor = v; }
    
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
    virtual void Zoom(const float delta);
    virtual void Translate(const glm::vec3 &delta);
    virtual void Rotate(const glm::vec2 &delta);
    virtual void Orbit(const glm::vec2 &delta);
    
    // Event handler
    // ----------------------------------------
    bool OnMouseScroll(MouseScrolledEvent &e);
    
    // Perspective camera variables
    // ----------------------------------------
protected:
    ///< Camera field of view (angle in degrees).
    float m_FieldOfView;
    
    ///< Camera movement scaling factors.
    float m_ZoomFactor = 0.25f;
    float m_TranslationFactor = 1.0f;
    float m_RotationFactor = 5.0f;
    float m_OrbitFactor = 12.0f;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PerspectiveCamera);
};

} // namespace pixc
