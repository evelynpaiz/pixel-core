#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A base class for materials with a simple texture-based shading.
 *
 * The `TextureProperty` class provides a basic material definition for shading 3D models with a
 * simple texture map.
 *
 * Inherit from this class when creating materials that have a single texture map.
 */
class TextureProperty
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the flat texture.
    virtual ~TextureProperty() = default;
    
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
    TextureProperty() = default;
    
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

} // namespace pixc
