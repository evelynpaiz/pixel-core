#pragma once

#include "Common/Renderer/Material/Material.h"

/**
 * A base class for materials with a simple color-based shading.
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
 * A base class for materials with a simple texture-based shading.
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
                       const std::string& name, unsigned int& slot)
    {
        shader->SetTexture(name, m_Texture, slot++);
    }
    
    // Flat texture variables
    // ----------------------------------------
protected:
    ///< Texture map.
    std::shared_ptr<Texture> m_Texture;
};

/**
 * A material class for simple color-based shading.
 *
 * The `SimpleColorMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a simple color. It uses a shader specified by the given file path
 * to apply the color to the model.
 *
 * Inherit from this class when creating materials that have a single albedo color.
 *
 * Copying or moving `SimpleColorMaterial` objects is disabled to ensure single ownership and
 * prevent unintended duplication of material resources.
 */
class SimpleColorMaterial : public Material, public FlatColor
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a material for hair with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    SimpleColorMaterial(const std::filesystem::path& filePath =
                        std::filesystem::path("Resources/shaders/base/SimpleColor"))
        : Material(filePath), FlatColor()
    {}
    /// @brief Destructor for the hair shading material.
    ~SimpleColorMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        FlatColor::SetProperties(m_Shader, "u_Material.Color");
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SimpleColorMaterial);
};

/**
 * A material class for simple texture-based shading.
 *
 * The `SimpleTextureMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a simple texture map. It uses a shader specified by the given file
 * path to apply the texture to the model.
 *
 * Inherit from this class when creating materials that have a single texture map.
 *
 * Copying or moving `SimpleTextureMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class SimpleTextureMaterial : public Material, public FlatTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a basic material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    SimpleTextureMaterial(const std::filesystem::path& filePath =
                  std::filesystem::path("Resources/shaders/base/SimpleTexture"))
        : Material(filePath), FlatTexture()
    {}
    /// @brief Destructor for the basic material.
    ~SimpleTextureMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        Material::SetMaterialProperties();
        FlatTexture::SetProperties(m_Shader, "u_Material.TextureMap", m_Slot);
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SimpleTextureMaterial);
};

/**
 * A material class that combines simple color and texture-based shading.
 *
 * The `SimpleMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a combination of a simple color and a texture map. It
 * uses a shader specified by the given file path to apply the material properties to the model.
 *
 * Inherit from this class when creating materials that have both an albedo color and a texture map.
 *
 * Copying or moving `SimpleMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class SimpleMaterial : public Material, public FlatColor, public FlatTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a basic material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    SimpleMaterial(const std::filesystem::path& filePath =
                  std::filesystem::path("Resources/shaders/base/SimpleColorTexture"))
        : Material(filePath), FlatColor(), FlatTexture()
    {}
    /// @brief Destructor for the basic material.
    ~SimpleMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        Material::SetMaterialProperties();
        FlatColor::SetProperties(m_Shader, "u_Material.Color");
        FlatTexture::SetProperties(m_Shader, "u_Material.TextureMap", m_Slot);
    }
    
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SimpleMaterial);
};
