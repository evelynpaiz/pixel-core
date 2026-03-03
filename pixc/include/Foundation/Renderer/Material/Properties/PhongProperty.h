#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/LitMaterial.h"
#include "Foundation/Renderer/Utils/BitMaskUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Flags controlling Phong shading variants.
 *
 * These flags can be combined using bitwise operators to specify multiple properties simultaneously.
 */
enum class PhongFlags : uint32_t
{
    None        = 0,        ///< No flags.
    HalfLambert = 1u << 0,  ///< Use half-Lambert diffuse.
    BlinnPhong  = 1u << 1   ///< Use Blinn–Phong specular instead of classic Phong.
};

// Enable bitwise operators for `PhongFlags`.
ENABLE_ENUM_BITMASK(PhongFlags);

/**
 * @brief A base class for implementing Phong shading properties.
 *
 * The `PhongProperty` class provides the core material properties required for
 * Phong shading. It defines the shininess (specular exponent) used by the Phong
 * lighting model and exposes functionality to upload this data to a shader.
 *
 * Inherit from this class when creating materials that rely on Phong-based lighting
 * calculations.
 */
class PhongProperty
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong base.
    virtual ~PhongProperty() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Enable a specific Phong flag.
    /// @param flag The flag to enable.
    void EnableFlag(PhongFlags flag) { m_Flags |= flag; }
    /// @brief Disable a specific Phong flag.
    /// @param flag The flag to disable.
    void DisableFlag(PhongFlags flag) { m_Flags &= ~flag; }
    
    /// @brief Set the shininess (exponent value).
    /// @param shininess The shininess value.
    void SetShininess(float shininess) { m_Shininess = shininess; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the shading flags.
    /// @return The bitmask of the Phong flags.
    PhongFlags GetFlags() const { return m_Flags; }
    /// @brief Check if a specific Phong flag is enabled.
    /// @param flag The flag to check.
    /// @return True if enabled, false otherwise.
    bool HasFlag(PhongFlags flag) const
    {
        return (m_Flags & flag) != PhongFlags::None;
    }
    
    /// @brief Get the shininess (exponent value).
    /// @return The shininess value.
    float GetShininess() const { return m_Shininess; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a phong base.
    PhongProperty() = default;
    
protected:
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetUint("u_Material.Flags", static_cast<uint32_t>(m_Flags));
        shader->SetFloat("u_Material.Shininess", m_Shininess);
    }
    
    // Phong properties
    // ----------------------------------------
protected:
    ///< Shading flags.
    PhongFlags m_Flags = PhongFlags::HalfLambert | PhongFlags::BlinnPhong;
    
    ///< Material shininess.
    float m_Shininess = 32.0f;
};

/**
 * @brief A property class for Phong shading with color-based lighting.
 *
 * The `PhongColorProperty` class defines the color-related coefficients used in
 * the Phong lighting model, including ambient, diffuse, and specular components,
 * as well as alpha transparency.
 *
 * Inherit from this class when creating materials that use Phong lighting driven
 * by constant color values rather than texture maps.
 */
class PhongColorProperty
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong color.
    virtual ~PhongColorProperty() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the albedo color.
    /// @param color Albedo color in RGBA.
    /// @brief Set the albedo color.
    /// @param color Albedo color in RGBA.
    virtual void SetColor (const glm::vec4& color)
    {
        // Set the color as the ambient and diffuse coefficients
        m_Ka = color;
        m_Kd = color;
        
        // Set the alpha value
        m_Alpha = color.a;
    }
    
    /// @brief Set the ambient coefficient for the material.
    /// @param k The ambient coefficient representing the RGB color components.
    void SetAmbientColor(const glm::vec3 &k) { m_Ka = k; }
    /// @brief Set the diffuse coefficient for the material.
    /// @param k The diffuse coefficient representing the RGB color components.
    void SetDiffuseColor(const glm::vec3 &k) { m_Kd = k; }
    /// @brief Set the specular coefficient for the material.
    /// @param k The specular coefficient representing the RGB color components.
    void SetSpecularColor(const glm::vec3 &k) { m_Ks = k; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the albedo color.
    /// @return The albedo color in RGBA.
    glm::vec4 GetColor() const { return glm::vec4(m_Kd, m_Alpha); }
    
    /// @brief Get the ambient coefficient of the material.
    /// @return The ambient coefficient representing the RGB color components.
    const glm::vec3& GetAmbientColor() const { return m_Ka; }
    /// @brief Get the diffuse coefficient of the material.
    /// @return The diffuse coefficient representing the RGB color components.
    const glm::vec3& GetDiffuseColor() const { return m_Kd; }
    /// @brief Get the specular coefficient of the material.
    /// @return The specular coefficient representing the RGB color components.
    const glm::vec3& GetSpecularColor() const { return m_Ks; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a phong color.
    PhongColorProperty() = default;
    
protected:
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetVec3("u_Material.Ka", m_Ka);
        shader->SetVec3("u_Material.Kd", m_Kd);
        shader->SetVec3("u_Material.Ks", m_Ks);
        
        shader->SetFloat("u_Material.Alpha", m_Alpha);
    }
    
    // Phong color properties
    // ----------------------------------------
protected:
    ///< Ambient coefficient.
    glm::vec3 m_Ka = glm::vec3(1.0f);
    ///< Diffuse coefficient.
    glm::vec3 m_Kd = glm::vec3(1.0f);
    ///< Specular coefficient.
    glm::vec3 m_Ks = glm::vec3(1.0f);
    
    ///< Alpha (transparency).
    float m_Alpha = 1.0f;
};

/**
 * @brief A property class for Phong shading with texture-based lighting.
 *
 * The `PhongTextureProperty` class defines texture-based inputs for the Phong
 * lighting model. It provides support for diffuse and specular texture maps and
 * handles binding these textures to the appropriate shader uniforms.
 *
 * Inherit from this class when creating materials that use texture maps to drive
 * Phong lighting.
 */
class PhongTextureProperty
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong texture.
    virtual ~PhongTextureProperty() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the diffuse texture map for the geometry.
    /// @param texture Texture map.
    virtual void SetDiffuseMap(const std::shared_ptr<Texture>& texture)
    {
        m_DiffuseTexture = texture;
    }
    /// @brief Set the specular texture map for the geometry.
    /// @param texture Texture map.
    virtual void SetSpecularMap(const std::shared_ptr<Texture>& texture)
    {
        m_SpecularTexture = texture;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the diffuse map used for the geometry.
    /// @return The diffuse texture map.
    std::shared_ptr<Texture> GetDiffuseMap() const { return m_DiffuseTexture; }
    /// @brief Get the specular map used for the geometry.
    /// @return The specular texture map.
    std::shared_ptr<Texture> GetSpecularMap() const { return m_SpecularTexture; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a phong texture.
    PhongTextureProperty() = default;
    
protected:
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetTexture("u_Material.DiffuseMap", m_DiffuseTexture,
                           static_cast<uint32_t>(TextureIndex::DiffuseMap));
        shader->SetTexture("u_Material.SpecularMap", m_SpecularTexture,
                           static_cast<uint32_t>(TextureIndex::SpecularMap));
    }
    
    // Phong texture properties
    // ----------------------------------------
protected:
    ///< Diffuse map.
    std::shared_ptr<Texture> m_DiffuseTexture;
    ///< Specular map.
    std::shared_ptr<Texture> m_SpecularTexture;
};

} // namespace pixc
