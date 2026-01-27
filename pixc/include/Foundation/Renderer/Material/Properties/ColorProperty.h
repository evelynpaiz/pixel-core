#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A base class for materials with a simple color-based shading.
 *
 * The `ColorProperty` class provides a basic material definition for shading 3D models with a
 * simple color.
 *
 * Inherit from this class when creating materials that have a single albedo color.
 */
class ColorProperty
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the flat color.
    virtual ~ColorProperty() = default;
    
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
    ColorProperty() = default;
    
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

} // namespace pixc
