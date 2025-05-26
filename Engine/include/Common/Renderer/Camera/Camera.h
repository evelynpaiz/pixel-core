#pragma once

#include "Common/Event/Event.h"
#include "Common/Core/Timestep.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace pixc;

/**
 * Represents a camera that captures the scene and displays it in a viewport.
 *
 * The `Camera` class provides an abstraction for a camera in a rendering engine. It captures the scene
 * and provides methods to access and manipulate camera properties. The camera's view matrix and
 * projection matrix are automatically updated when camera properties change.
 *
 * Copying or moving `Camera` objects is disabled to ensure single ownership and prevent unintended
 * camera duplication.
 */
class Camera
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the camera.
    virtual ~Camera() = default;
    
    // Rendering
    // ----------------------------------------
    /// @brief Update the camera information in the current frame.
    /// @param ts Timestep.
    virtual void OnUpdate(Timestep ts) {}
    
    // Event handler
    // ----------------------------------------
    /// @brief Handle an event on the application related to the camera.
    /// @param e Event.
    virtual void OnEvent(Event &e) {}
    
    // Getter(s)
    // ----------------------------------------
    glm::quat GetOrientation() const;
    glm::vec3 GetUpDirection() const;
    glm::vec3 GetRightDirection() const;
    glm::vec3 GetFowardDirection() const;
    
    /// @brief Get the camera resolution (width).
    /// @return Viewport width.
    unsigned int GetWidth() const { return m_Width; }
    /// @brief Get the camera resolution (height).
    /// @return Viewport height.
    unsigned int GetHeight() const { return m_Height; }
    /// @brief Get the camera aspect ratio.
    /// @return The aspect ratio (width/height).
    float GetAspectRatio() const
    {
        return (float)m_Width / (float)m_Height;
    }
    /// @brief Get the distance from the camera position to the near plane.
    /// @return The near plane distance.
    float GetNearPlane() const { return m_NearPlane; }
    /// @brief Get the distance from the camera position to the far plane.
    /// @return The far plane distance.
    float GetFarPlane() const { return m_FarPlane; }
    
    /// @brief Get the camera position (x, y, z).
    /// @return The camera position coordinates.
    const glm::vec3& GetPosition() const { return m_Position; }
    /// @brief Get the camera orientation (yaw, pitch, roll).
    /// @return The camera rotation angles.
    const glm::vec3& GetRotation() const { return m_Rotation; }
    
    /// @brief Get the camera view matrix (transformation from world space to view/camera space).
    /// @return The view matrix.
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    /// @brief Get the camera projection matrix (transformation from view/camera space to screen space).
    /// @return The projection matrix.
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    
    /// @brief Get the camera target coordinates (x, y, z).
    /// @return Camera target.
    const glm::vec3& GetTarget() const { return m_Target; }
    
    // Setter(s)
    // ----------------------------------------
    void SetViewportSize(const int width, const int height);
    
    /// @brief Change the distance from the camera position to the near plane.
    /// @param nearPlane Distance to the near plane.
    void SetNearPlane(const float nearPlane)
    {
        m_NearPlane = nearPlane;
        UpdateProjectionMatrix();
    }
    /// @brief Change the distance from the camera position to the far plane.
    /// @param farPlane Distance to the far plane.
    void SetFarPlane(const float farPlane)
    {
        m_FarPlane = farPlane;
        UpdateProjectionMatrix();
    }
    
    /// @brief Change the camera position (x, y, z).
    /// @param position The camera center position.
    void SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        UpdateViewMatrix();
    }
    /// @brief Change the camera orientation (yaw, pitch, roll).
    /// @param orientation The camera rotation angles.
    void SetRotation(const glm::vec3& rotation)
    {
        m_Rotation = rotation;
        UpdateViewMatrix();
    }
    
    /// @brief Change the camera target coordinates.
    /// @param target The camera target (x, y, z).
    void SetTarget(const glm::vec3& target)
    {
        m_Target = target;
        UpdateViewMatrix();
    }
    
    /// @brief Enable the camera for interaction.
    /// @param e Enable/disable the camera.
    void Enable(const bool e) { m_Enabled = e; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a camera.
    /// @param width Viewport size (width).
    /// @param height Viewport size (height).
    /// @param near Distance to the near plane.
    /// @param far Distance to the far plane.
    Camera(const int width, const int height, const float nearPlane, const float farPlane)
        : m_Width(width), m_Height(height), m_NearPlane(nearPlane), m_FarPlane(farPlane)
    {}
    
    // Getter(s)
    // ----------------------------------------
    float CalculatePitch() const;
    float CalculateYaw() const;
    
    // Transformation matrices
    // ----------------------------------------
    virtual void UpdateViewMatrix();
    virtual void UpdateProjectionMatrix();
    void UpdateCameraMatrices();
    
    // Camera variables
    // ----------------------------------------
protected:
    ///< Camera resolution.
    unsigned int m_Width, m_Height;
    
    ///< Distance to near and far plane.
    float m_NearPlane, m_FarPlane;
    
    ///< Camera position (x, y, z).
    glm::vec3 m_Position = glm::vec3(0.0f);
    /// Camera orientation (pitch, yaw, and roll angles).
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    
    ///< View matrix.
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    ///< Projection matrix.
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    
    ///< Camera viewing target (x, y, z).
    glm::vec3 m_Target = glm::vec3(0.0f);
    
    ///< Camera enabled for interaction.
    bool m_Enabled = true;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Camera);
};
