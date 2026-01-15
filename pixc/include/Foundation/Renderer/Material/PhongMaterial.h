#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/LightedMaterial.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A base class for implementing Phong shading properties.
 *
 * The `Phong` class provides a base class for defining material properties used in Phong
 * shading. It is intended to be inherited by specific material classes to add Phong support.
 */
class Phong
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong base.
    virtual ~Phong() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the shininess (exponent value).
    /// @param shininess The shininess value.
    void SetShininess(float shininess) { m_Shininess = shininess; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the shininess (exponent value).
    /// @return The shininess value.
    float GetShininess() const { return m_Shininess; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a phong base.
    Phong() = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetFloat("u_Material.Shininess", m_Shininess);
    }
    
    // Flat color variables
    // ----------------------------------------
protected:
    ///< Material shininess.
    float m_Shininess = 32.0f;
};

/**
 * @brief A subclass of Phong implementing Phong shading with color-based lighting.
 *
 * The `PhongColor` class is a subclass of `Phong` and provides a material definition for
 * shading 3D models using Phong lighting with color-based illumination. It allows setting the
 * ambient, diffuse, and specular coefficients for the material and provides methods to get
 * these coefficients.
 */
class PhongColor : public Phong
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong color.
    virtual ~PhongColor() = default;
    
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
    PhongColor() = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetVec3("u_Material.Ka", m_Ka);
        shader->SetVec3("u_Material.Kd", m_Kd);
        shader->SetVec3("u_Material.Ks", m_Ks);
        
        shader->SetFloat("u_Material.Alpha", m_Alpha);
        
        Phong::SetProperties(shader);
    }
    
    // Phong color variables
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
 * @brief A subclass of Phong implementing Phong shading with texture-based lighting.
 *
 * The `PhongTexture` class is a subclass of `Phong` and provides a material definition for
 * shading 3D models using Phong lighting with texture-based illumination. It allows setting the
 * diffuse and specular texture maps for the material and provides methods to get these texture
 * maps.
 */
class PhongTexture : public Phong
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the phong texture.
    virtual ~PhongTexture() = default;
    
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
    PhongTexture() = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    /// @param shader The shader program to set the properties for.
    void SetProperties(const std::shared_ptr<Shader>& shader)
    {
        shader->SetTexture("u_Material.DiffuseMap", m_DiffuseTexture,
                           static_cast<uint32_t>(TextureIndex::DiffuseMap));
        shader->SetTexture("u_Material.SpecularMap", m_SpecularTexture,
                           static_cast<uint32_t>(TextureIndex::SpecularMap));
        
        Phong::SetProperties(shader);
    }
    
    // Phong color variables
    // ----------------------------------------
protected:
    ///< Diffuse map.
    std::shared_ptr<Texture> m_DiffuseTexture;
    ///< Specular map.
    std::shared_ptr<Texture> m_SpecularTexture;
};

/**
 * @brief A material class for Phong shading with color-based lighting.
 *
 * The `PhongColorMaterial` class is a subclass of `LightedMaterial` and `PhongColor`,
 * providing a material definition for shading 3D models using Phong lighting with color-based
 * illumination. It uses a shader specified by the given file path to apply the shading to the model.
 *
 * Copying or moving `PhongColorMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class PhongColorMaterial : public LightedMaterial, public PhongColor
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a phong material object with the specified shader file path.
    /// @param light The light source to be used for shading.
    /// @param filePath The file path to the shader used by the material.
    PhongColorMaterial(const std::filesystem::path& filePath =
                       ResourcesManager::GeneralPath("pixc/shaders/forward/lit/phong/PhongColor"))
    : LightedMaterial(filePath), PhongColor()
    {
        // Update material properties
        m_Properties = MaterialProperty::ViewDirection | MaterialProperty::NormalMatrix;
    }
    /// @brief Destructor for the phong color material.
    ~PhongColorMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        LightedMaterial::SetMaterialProperties();
        PhongColor::SetProperties(m_Shader);
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PhongColorMaterial);
};

/**
 * @brief A material class for Phong shading with texture-based lighting.
 *
 * The `PhongTextureMaterial` class is a subclass of `LightedMaterial` and `PhongTexture`,
 * providing a material definition for shading 3D models using Phong lighting with texture-based
 * illumination. It uses a shader specified by the given file path to apply the shading to the model.
 *
 * Copying or moving `PhongTextureMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class PhongTextureMaterial : public LightedMaterial, public PhongTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a phong material object with the specified shader file path.
    /// @param environment The light source to be used for shading.
    /// @param filePath The file path to the shader used by the material.
    PhongTextureMaterial(const std::filesystem::path& filePath =
                         ResourcesManager::GeneralPath("pixc/shaders/forward/lit/phong/PhongTexture"))
    : LightedMaterial(filePath), PhongTexture()
    {
        // Update material properties
        m_Properties = MaterialProperty::ViewDirection | MaterialProperty::NormalMatrix;
    }
    /// @brief Destructor for the phong texture material.
    ~PhongTextureMaterial() override = default;
    
protected:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        LightedMaterial::SetMaterialProperties();
        PhongTexture::SetProperties(m_Shader);
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PhongTextureMaterial);
};

} // namespace pixc
