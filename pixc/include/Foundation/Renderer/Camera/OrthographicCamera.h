#pragma once

#include <glm/glm.hpp>

#include "Foundation/Renderer/Camera/Camera.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class MouseScrolledEvent;

/**
 * @brief Represents an orthographic camera that captures the scene without perspective distortion.
 *
 * The `OrthographicCamera`  class is a derived class of the `Camera` class, providing an orthographic
 * projection for rendering. It is typically used for 2D scenes or UI rendering, where objects should retain
 * their size regardless of depth. It inherits the functionality of the base `Camera` class and offers
 * additional methods to control orthographic bounds and zoom levels. The camera's view matrix and
 * projection matrix are automatically updated when camera properties change.
 *
 * Copying or moving `OrthographicCamera` objects is disabled to ensure single ownership and
 * prevent unintended camera duplication.
 */
class OrthographicCamera : public Camera
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    OrthographicCamera(const int width, const int height, const float nearPlane = 0.1f,
                       const float farPlane = 100.0f);
    
    // Rendering
    // ----------------------------------------
    void OnUpdate(Timestep ts) override;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the camera rotation angle in the z-axis.
    /// @return Roll angle (degrees).
    float GetRotationAngle() const { return m_Rotation.z; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Change the camera orthographic size.
    /// @param size The orthographic size of the camera.
    void SetOrthographicSize(const float size)
    {
        m_OrthoSize = size;
        UpdateProjectionMatrix();
    }
    
    /// @brief Change the camera rotation in the z-axis.
    /// @param roll The camera rotation angle.
    void SetRotationAngle(const float roll)
    {
        m_Rotation.z = roll;
        UpdateViewMatrix();
    }
    
    /// @brief Update the camera translation scaling factor.
    /// @param v Scaling factor value.
    void SetTranslateFactor(const float v) { m_TranslationFactor = v; }
    /// @brief Update the camera rotation scaling factor.
    /// @param v Scaling factor value.
    void SetRotateFactor(const float v) { m_RotationFactor = v; }
    
protected:
    // Transformation matrices
    // ----------------------------------------
    void UpdateViewMatrix() override;
    void UpdateProjectionMatrix() override;
    
    // Camera movements
    // ----------------------------------------
    virtual void Translate(const glm::vec2 &delta);
    virtual void Rotate(const glm::vec2 &delta);
    
    // Perspective camera variables
    // ----------------------------------------
protected:
    ///< View size.
    float m_OrthoSize = 10.0f;
    
    ///< Camera movement scaling factors.
    float m_TranslationFactor = 2.0f;
    float m_RotationFactor = 7.0f;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OrthographicCamera);
};

} // namespace pixc
