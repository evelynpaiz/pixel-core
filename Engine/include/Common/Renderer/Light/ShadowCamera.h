#pragma once

#include "Common/Renderer/Camera/OrthographicCamera.h"
#include "Common/Renderer/Camera/PerspectiveCamera.h"

using namespace pixc;

/**
 * Represents an orthographic shadow camera projection.
 *
 * The `OrthographicShadow` class extends the `OrthographicCamera` class to define a
 * camera projection suitable for rendering shadows. It inherits the functionality of the orthographic
 * camera and updates the view matrix accordingly for shadow calculations.
 */
class OrthographicShadow : public OrthographicCamera
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Generate an orthographic shadow camera with default values.
    OrthographicShadow() : OrthographicCamera(1, 1) {}
    /// @brief Destructs the orthographic shadow camera.
    virtual ~OrthographicShadow() = default;
    
    // Transformation matrices
    // ----------------------------------------
    /// @brief Update the camera view matrix.
    /// This method updates the camera's view matrix based on its position and target, using the
    /// `CalculateLookAtMatrix` utility function.
    void UpdateViewMatrix() override
    {
        // Set the new rotation angles
        m_Rotation.x = Camera::CalculatePitch();
        m_Rotation.y = Camera::CalculateYaw();
        
        Camera::UpdateViewMatrix();
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OrthographicShadow);
};

/**
 * Represents a perspective shadow camera projection.
 *
 * The `PerspectiveShadow` class extends the `PerspectiveCamera` class to define a
 * camera projection suitable for rendering shadows. It inherits the functionality of the perspective
 * camera and updates the view matrix accordingly for shadow calculations.
 */
class PerspectiveShadow : public PerspectiveCamera
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Generate a perspective shadow camera with default values.
    PerspectiveShadow() : PerspectiveCamera(1, 1) {}
    /// @brief Destructs the orthographic shadow camera.
    virtual ~PerspectiveShadow() = default;
    
    // Transformation matrices
    // ----------------------------------------
    /// @brief Update the camera view matrix.
    /// This method updates the camera's view matrix based on its position and target, using the
    /// `CalculateLookAtMatrix` utility function.
    void UpdateViewMatrix() override
    {
        // Set the new rotation angles
        m_Rotation.x = Camera::CalculatePitch();
        m_Rotation.y = Camera::CalculateYaw();
        
        Camera::UpdateViewMatrix();
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PerspectiveShadow);
};
