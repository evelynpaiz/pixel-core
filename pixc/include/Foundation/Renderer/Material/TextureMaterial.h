#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/Material.h"
#include "Foundation/Renderer/Material/Properties/TextureProperty.h"

#include "Foundation/Renderer/Texture/Texture2D.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A material class for simple texture-based shading.
 *
 * The `TextureMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a simple texture map. It uses a shader specified by the given file
 * path to apply the texture to the model.
 *
 * Inherit from this class when creating materials that have a single texture map.
 *
 * Copying or moving `TextureMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class TextureMaterial : public Material, public TextureProperty
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a texture material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    TextureMaterial(const std::filesystem::path& filePath)
    : Material(filePath), TextureProperty()
    {}
    /// @brief Destructor for the texture material.
    ~TextureMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        Material::SetMaterialProperties();
        TextureProperty::SetProperties(m_Shader, m_TextureUniformName,
                                       static_cast<uint32_t>(TextureIndex::TextureMap));
    }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Update the texture uniform name.
    /// @param name Texture uniform name.
    void SetTextureUniformName(const std::string& name)
    {
        m_TextureUniformName = name;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the texture uniform name
    /// @return The texture uniform name.
    const std::string& GetTextureUniformName() const
    {
        return m_TextureUniformName;
    }
    
private:
    ///< Uniform name to bind the texture
    std::string m_TextureUniformName = "u_Material.TextureMap";
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(TextureMaterial);
};

} // namespace pixc
