#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/Material.h"
#include "Foundation/Renderer/Texture/Texture2D.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A base class for materials with a simple color-based shading.
 *
 * The `FlatColor` class provides a basic material definition for shading 3D models with a
 * simple color.
 *
 * Inherit from this class when creating materials that have a single albedo color.
 */
class FlatColor
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the flat color.
    virtual ~FlatColor() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the albedo color.
    /// @param color Albedo color in RGBA.
    virtual void SetColor(const glm::vec4& color) { m_Color = color; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the albedo color.
    /// @return The albedo color in RGBA.
    glm::vec4 GetColor() const { return m_Color; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a flat color.
    FlatColor() = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    /// @param name The uniform name.
    void SetProperties(const std::shared_ptr<Shader>& shader,
                       const std::string& name)
    {
        shader->SetVec4(name, m_Color);
    }
    
    // Flat color variables
    // ----------------------------------------
protected:
    ///< Albedo color.
    glm::vec4 m_Color = glm::vec4(1.0f);
};

/**
 * @brief A base class for materials with a simple texture-based shading.
 *
 * The `FlatTexture` class provides a basic material definition for shading 3D models with a
 * simple texture map.
 *
 * Inherit from this class when creating materials that have a single texture map.
 */
class FlatTexture
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the flat texture.
    virtual ~FlatTexture() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the texture map for the geometry.
    /// @param texture Texture map.
    virtual void SetTextureMap(const std::shared_ptr<Texture>& texture)
    {
        m_Texture = texture;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the texture map used for the geometry.
    /// @return The texture map.
    std::shared_ptr<Texture> GetTextureMap() const { return m_Texture; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a flat texture.
    FlatTexture() = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    /// @param name The uniform name.
    void SetProperties(const std::shared_ptr<Shader>& shader,
                       const std::string& name, uint32_t slot)
    {
        shader->SetTexture(name, m_Texture, slot);
    }
    
    // Flat texture variables
    // ----------------------------------------
protected:
    ///< Texture map.
    std::shared_ptr<Texture> m_Texture;
};

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
class TextureMaterial : public Material, public FlatTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a basic material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    TextureMaterial(const std::filesystem::path& filePath)
    : Material(filePath), FlatTexture()
    {}
    /// @brief Destructor for the basic material.
    ~TextureMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        Material::SetMaterialProperties();
        FlatTexture::SetProperties(m_Shader, m_TextureUniformName,
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

/**
 * @brief A material class that combines simple color and texture-based shading.
 *
 * The `UnlitMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a combination of a simple color and a texture map. It
 * uses a shader specified by the given file path to apply the material properties to the model.
 *
 * Inherit from this class when creating materials that have both an albedo color and a texture map.
 *
 * Copying or moving `UnlitMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class UnlitMaterial : public TextureMaterial, public FlatColor
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a simple material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    UnlitMaterial(const std::filesystem::path& filePath =
                   ResourcesManager::GeneralPath("pixc/shaders/forward/unlit/Simple"))
    : TextureMaterial(filePath)
    {
        m_Texture = pixc::utils::textures::WhiteTexture2D();
    }
    /// @brief Destructor for the basic material.
    ~UnlitMaterial() override = default;
    
private:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        TextureMaterial::SetMaterialProperties();
        FlatColor::SetProperties(m_Shader, "u_Material.Color");
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(UnlitMaterial);
};

} // namespace pixc
