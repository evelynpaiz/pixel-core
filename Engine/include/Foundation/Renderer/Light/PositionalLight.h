#pragma once

#include "Common/Renderer/Light/Light.h"

#include "Common/Renderer/Material/SimpleMaterial.h"

#include "Common/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Common/Renderer/Drawable/Model/ModelUtils.h"

#include <glm/glm.hpp>

/**
 * Represents a positional light source in 3D rendering.
 *
 * The `PositionalLight` class extends the `Light` base class to define a positional light source.
 * It provides methods to set and retrieve the light's position, as well as additional properties
 * such as the shadow camera and 3D model representation.
 *
 * Copying or moving `PositionalLight` objects is disabled to ensure single ownership and prevent
 * unintended duplication of light resources.
 */
class PositionalLight : public Light
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a light source with a position in the world space.
    /// @param width The width that the light source covers.
    /// @param height The height that the light source covers.
    /// @param color The color of the light source.
    /// @param position The position of the light source.
    /// @param angle The angle of visibility of the light source.
    PositionalLight(const unsigned int width, const unsigned int height,
                    const glm::vec3 &color = glm::vec3(1.0f),
                    const glm::vec3 &position = glm::vec3(0.0f),
                    float angle = 90.0f)
        : Light(glm::vec4(position, 1.0f), color)
    {
        // Set the shadow camera parameters
        auto shadowCamera = std::make_shared<PerspectiveShadow>();
        shadowCamera->SetViewportSize(width, height);
        shadowCamera->SetPosition(position);
        shadowCamera->SetFieldOfView(angle);
        m_ShadowCamera = shadowCamera;
        InitShadowMapBuffer(width, height);
        
        // Define the 3D model for the positional light
        std::shared_ptr<SimpleColorMaterial> material = std::make_shared<SimpleColorMaterial>();
        material->SetColor(glm::vec4(color, 1.0f));
        
        using VertexData = GeoVertexData<glm::vec4>;
        m_Model = utils::Geometry::ModelSphere<VertexData>(material);
        m_Model->SetScale(glm::vec3(0.25f));
        m_Model->SetPosition(position);
    }
    
    /// @brief Destructor for the positional light.
    ~PositionalLight() override = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Change the light position (x, y, z).
    /// @param position The light center position.
    void SetPosition(const glm::vec3& position) {
        m_Vector = glm::vec4(position, 1.0f);
        m_ShadowCamera->SetPosition(position);
        m_Model->SetPosition(position);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the light position (x, y, z).
    /// @return The light position coordinates.
    glm::vec3 GetPosition() const { return m_Vector; }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PositionalLight);
};
