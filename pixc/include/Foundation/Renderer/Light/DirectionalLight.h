#pragma once

#include "Foundation/Renderer/Light/Light.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a directional light source in 3D rendering.
 *
 * The `DirectionalLight` class extends the `Light` base class to define a directional light source.
 * It provides methods to set and retrieve the light's direction, distance, and additional properties
 * such as the shadow camera.
 *
 * Copying or moving `DirectionalLight` objects is disabled to ensure single ownership and prevent
 * unintended duplication of light resources.
 */
class DirectionalLight : public LightCaster
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a light source defined by a direction.
    /// @param color The color of the light source.
    /// @param direction The direction of the light source.
    DirectionalLight(const glm::vec3& color = glm::vec3(1.0f),
                     const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f))
    : LightCaster(glm::vec4(direction, 0.0f), color)
    {
        // Set the shadow camera parameters
        auto camera = std::make_shared<OrthographicShadow>();
        m_Shadow.Camera = camera;
        
        UpdateShadowCamera();
    }
    /// @brief Destructor for the directional light.
    ~DirectionalLight() override = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Change the light direction.
    /// @param direction The light direction.
    void SetDirection(const glm::vec3& direction)
    {
        m_Vector = glm::vec4(direction, 0.0f);
        UpdateShadowCamera();
    }
    /// @brief Change the distance of the light.
    /// @param distance The light distance.
    void SetDistance(const float distance)
    {
        m_Distance = distance;
        UpdateShadowCamera();
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the light direction.
    /// @return The light direction vector.
    glm::vec3 GetDirection() const { return m_Vector; }
    /// @brief Get the distance of the light source.
    /// @return The light distance.
    float GetDistance() const { return m_Distance; }
    
private:
    // Update(s)
    // ----------------------------------------
    /// @brief Update the shadow camera based on the properties of the light source.
    void UpdateShadowCamera()
    {
        // Update the position of the camera
        glm::vec3 position = m_Shadow.Camera->GetTarget() - (glm::normalize(glm::vec3(m_Vector)) * m_Distance);
        m_Shadow.Camera->SetPosition(position);
        
        // Update the zoom factor of the camera to capture everything
        m_Shadow.Camera->SetZoomFactor(m_Distance);
    }
    
    // Light variables
    // ----------------------------------------
private:
    ///< The distance from the (shadow camera) target to the light source.
    float m_Distance = 1.0f;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(DirectionalLight);
};

} // namespace pixc
