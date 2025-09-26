#pragma once

#include "Foundation/Renderer/Light/Light.h"

#include "Foundation/Renderer/Material/SimpleMaterial.h"

#include "Foundation/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a positional light source in 3D rendering.
 *
 * The `PositionalLight` class extends the `Light` base class to define a positional light source.
 * It provides methods to set and retrieve the light's position, as well as additional properties
 * such as the shadow camera and 3D model representation.
 *
 * Copying or moving `PositionalLight` objects is disabled to ensure single ownership and prevent
 * unintended duplication of light resources.
 */
class PositionalLight : public LightCaster
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a light source with a position in the world space.
    /// @param color The color of the light source.
    /// @param position The position of the light source.
    /// @param angle The angle of visibility of the light source.
    PositionalLight(const glm::vec3 &color = glm::vec3(1.0f),
                    const glm::vec3 &position = glm::vec3(0.0f),
                    float angle = 90.0f)
        : LightCaster(glm::vec4(position, 1.0f), color)
    {
        // Set the shadow camera parameters
        auto camera = std::make_shared<PerspectiveShadow>();
        camera->SetPosition(position);
        camera->SetFieldOfView(angle);
        m_Shadow.Camera = camera;
        
        // Define the 3D model for the positional light
        using VertexData = GeoVertexData<glm::vec4>;
        m_Model = utils::geometry::ModelSphere<VertexData>(GetOrCreateLightMaterial());
        m_Model->SetPosition(position);
        // TODO: needs to be defined based on the type of the scene
        m_Model->SetScale(glm::vec3(0.25f));
    }
    
    /// @brief Destructor for the positional light.
    ~PositionalLight() override = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Change the light position (x, y, z).
    /// @param position The light center position.
    void SetPosition(const glm::vec3& position)
    {
        m_Vector = glm::vec4(position, 1.0f);
        m_Shadow.Camera->SetPosition(position);
        m_Model->SetPosition(position);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the light position (x, y, z).
    /// @return The light position coordinates.
    glm::vec3 GetPosition() const { return m_Vector; }
    
    // Render
    // ----------------------------------------
    /// @brief Renders the 3D model that represents the light source.
    void DrawLight() override
    {
        auto material = GetOrCreateLightMaterial();
        material->SetColor(glm::vec4(m_Color, 1.0f));
        Light::DrawLight();
    }
    
private:
    /// @brief Ensures the "LightMaterial" exists in the material library and returns it.
    /// @return Shared pointer to the `SimpleColorMaterial` instance.
    std::shared_ptr<SimpleColorMaterial> GetOrCreateLightMaterial()
    {
        auto& library = Renderer::GetMaterialLibrary();
        if (!library.Exists("LightMaterial"))
        {
            library.Create<SimpleColorMaterial>("LightMaterial");
        }
        return std::dynamic_pointer_cast<SimpleColorMaterial>(library.Get("LightMaterial"));
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PositionalLight);
};

} // namespace pixc
