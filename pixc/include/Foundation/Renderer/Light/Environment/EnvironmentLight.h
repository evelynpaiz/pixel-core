#pragma once

#include "Foundation/Renderer/Light/Light.h"

#include "Foundation/Renderer/Buffer/FrameBuffer.h"
#include "Foundation/Renderer/Texture/Texture.h"
#include "Foundation/Renderer/Material/Material.h"

#include "Foundation/Renderer/RendererCommand.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents an environment light source in 3D rendering.
 *
 * The `EnvironmentLight` class extends the `Light` base class to define an environment light source.
 * It provides methods to set and retrieve the light's position, environment map, and additional properties
 * such as the shadow camera and 3D model representation.
 *
 * Copying or moving `EnvironmentLight` objects is disabled to ensure single ownership and prevent
 * unintended duplication of light resources.
 */
class EnvironmentLight : public Light
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    EnvironmentLight(const unsigned int size = 2048);
    
    /// @brief Destructor for the environment light.
    virtual ~EnvironmentLight() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the strength of the ambient light.
    /// @param s The strength of the ambient component (a value between 0 and 1).
    void SetAmbientStrength(float s) { m_AmbientStrength = s; }
    
    virtual void SetEnvironmentMap(const std::shared_ptr<Texture>& texture);
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the ambient light strength.
    /// @return The ambient strength.
    float GetAmbientStrength() const { return m_AmbientStrength; }
    
    /// @brief Get the environment map.
    /// @return The texture describing the environment.
    const std::shared_ptr<Texture>& GetEnvironmentMap() { return m_EnvironmentMap; }
    
    //const std::shared_ptr<Texture>& GetIrradianceMap();
    //const std::shared_ptr<Texture>& GetPreFilterMap();
    
    // Render
    // ----------------------------------------
    /// @brief Renders the 3D model that represents the light source.
    void DrawLight() override;
    
protected:
    // Initialization
    // ----------------------------------------
    void InitEnvironment(const unsigned int size);
    
    virtual void SetupFramebuffers(const unsigned int size);
    virtual void SetupResources();
    
    // Properties
    // ----------------------------------------
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               LightProperty properties) override;
    
    // Update
    // ----------------------------------------
    virtual void UpdateEnvironment();
    
    // Environment variables
    // ----------------------------------------
protected:
    ///< Framebuffer(s) for pre-processing.
    FrameBufferLibrary m_Framebuffers;
    
    ///< The ambient light intensity.
    float m_AmbientStrength = 0.4f;
    ///< The environment map.
    std::shared_ptr<Texture> m_EnvironmentMap;
    /// Environment orientation (pitch, yaw, and roll angles).
    glm::vec3 m_Rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(EnvironmentLight);
};

} // namespace pixc
