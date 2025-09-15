#pragma once

#include "Foundation/Renderer/Light/Light.h"

#include "Foundation/Renderer/Buffer/FrameBuffer.h"
#include "Foundation/Renderer/Texture/Texture.h"
#include "Foundation/Renderer/Material/Material.h"

#include "Foundation/Renderer/RendererCommand.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Represents an environment light source in 3D rendering.
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
    ~EnvironmentLight() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the strength of the ambient light.
    /// @param s The strength of the ambient component (a value between 0 and 1).
    void SetAmbientStrength(float s) { m_AmbientStrength = s; }
    
    void SetEnvironmentMap(const std::shared_ptr<Texture>& texture);
    
    /// @brief Set the size of the environment cube used for rendering the skybox.
    /// @param size The new environment cube size in world units.
    void SetEnvironmentSize(float size) { m_Size = size; }
    
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
    
    /// @brief Get the size of the environment cube used for rendering the skybox.
    /// @return The current environment cube size in world units.
    float GetEnvironmentSize() const { return m_Size; }
    
    // Render
    // ----------------------------------------
    /// @brief Renders the 3D model that represents the light source.
    void DrawLight() override;
    
private:
    // Initialization
    // ----------------------------------------
    void InitEnvironment(const unsigned int size);
    
    void SetupFramebuffers(const unsigned int size);
    void SetupResources();
    
    // Properties
    // ----------------------------------------
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               LightProperty properties,
                               unsigned int& slot) override;
    
    // Update
    // ----------------------------------------
    void UpdateEnvironment();
    //void UpdateSphericalHarmonics();
    
    // Environment variables
    // ----------------------------------------
private:
    ///< The ambient light intensity.
    float m_AmbientStrength = 0.4f;
    
    ///< Framebuffer(s) for pre-processing.
    FrameBufferLibrary m_Framebuffers;
    
    ///< The environment map.
    std::shared_ptr<Texture> m_EnvironmentMap;
    
    ///< Environment size.
    float m_Size = 80.0f;
    /// Environment orientation (pitch, yaw, and roll angles).
    glm::vec3 m_Rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    
    ///< Spherical harmonics coefficients.
    //SHCoefficients m_Coefficients;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(EnvironmentLight);
};

/**
 * Represents spherical harmonic (SH) coefficients for isotropic and anisotropic irradiance.
 
struct SHCoefficients
{
 */
    /**
     * A structure representing a 4x4 matrix for each color channel (red, green, blue).
     *
    struct SHMatrix
    {
        glm::mat4 Red = glm::mat4(0.0f);    ///< 4x4 matrix for the red channel.
        glm::mat4 Green = glm::mat4(0.0f);  ///< 4x4 matrix for the green channel.
        glm::mat4 Blue = glm::mat4(0.0f);   ///< 4x4 matrix for the blue channel.
    };
    
    // Matrix Computation
    // ----------------------------------------
    void UpdateIsotropicMatrix(const std::vector<float>& shCoeffs);
    void UpdateAnisotropicMatrix(const std::vector<float>& shCoeffs);
    
    static glm::mat4 GenerateIsotropicMatrix(int colorIndex,
                                             const std::vector<float>& shCoeffs);
    static glm::mat4 GenerateAnisotropicMatrix(int colorIndex,
                                               const std::vector<float>& shCoeffs);
    
    // Spherical Harmonics
    // ----------------------------------------
    SHMatrix Isotropic;         ///< SH coefficients for isotropic irradiance (using normals).
    SHMatrix Anisotropic;       ///< SH coefficients for anisotropic irradiance (using tangents).
};
     **/

} // namespace pixc
