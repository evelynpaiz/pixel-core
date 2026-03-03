#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/LitMaterial.h"
#include "Foundation/Renderer/Material/Properties/PhongProperty.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class PhongMaterial : public LitMaterial,
                      public PhongProperty,
                      public PhongColorProperty,
                      public PhongTextureProperty
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a phong material object with the specified shader file path.
    /// @param light The light source to be used for shading.
    /// @param filePath The file path to the shader used by the material.
    PhongMaterial(const std::filesystem::path& filePath =
                  ResourcesManager::GeneralPath("pixc/shaders/forward/lit/phong/LitPhong"))
    : LitMaterial(filePath), PhongProperty(), PhongColorProperty(), PhongTextureProperty()
    {
        // Update general material properties
        m_Properties = MaterialProperty::ViewDirection | MaterialProperty::NormalMatrix;
        // Set the diffuse and specular maps to a white texture
        m_DiffuseTexture = pixc::utils::textures::WhiteTexture2D();
        m_SpecularTexture = pixc::utils::textures::WhiteTexture2D();
    }
    /// @brief Destructor for the phong color material.
    ~PhongMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        // Define the phong material properties
        PhongProperty::SetProperties(m_Shader);
        PhongColorProperty::SetProperties(m_Shader);
        PhongTextureProperty::SetProperties(m_Shader);
        // Define the lit material properties
        LitMaterial::SetMaterialProperties();
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PhongMaterial);
};

} // namespace pixc
