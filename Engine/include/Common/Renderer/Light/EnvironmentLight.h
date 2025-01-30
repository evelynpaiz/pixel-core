#pragma once

#include "Common/Renderer/Texture/Texture3D.h"

#include "Common/Renderer/Light/Light.h"

#include "Common/Renderer/Material/SimpleMaterial.h"

#include "Common/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Common/Renderer/Drawable/Model/ModelUtils.h"

#include <glm/glm.hpp>

/**
 * Represents spherical harmonic (SH) coefficients for isotropic and anisotropic irradiance.
 */
struct SHCoefficients
{
    /**
     * A structure representing a 4x4 matrix for each color channel (red, green, blue).
     */
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
class EnvironmentLight : public BaseLight
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    EnvironmentLight(const unsigned int width, const unsigned int height);
    
    /// @brief Destructor for the environment light.
    ~EnvironmentLight() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the strength of the ambient light.
    /// @param s The strength of the ambient component (a value between 0 and 1).
    void SetAmbientStrength(float s) { m_AmbientStrength = s; }
    
    void SetEnvironmentMap(const std::shared_ptr<Texture>& texture);
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the ambient light strength.
    /// @return The ambient strength.
    float GetAmbientStrength() const { return m_AmbientStrength; }
    
    /// @brief Get the environment map.
    /// @return The texture describing the environment.
    const std::shared_ptr<Texture>& GetEnvironmentMap() { return m_EnvironmentMap; }
    
    const std::shared_ptr<Texture>& GetIrradianceMap();
    const std::shared_ptr<Texture>& GetPreFilterMap();
    
    // Render
    // ----------------------------------------
    /// @brief Renders the 3D model that represents the light source.
    void DrawLight() override
    {
        if (m_Model && m_EnvironmentMap)
        {
            Renderer::SetDepthFunction(DepthFunction::LEqual);
            m_Model->DrawModel();
            Renderer::SetDepthFunction(DepthFunction::Less);
        }
    }
    
private:
    // Properties
    // ----------------------------------------
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               const LightFlags& flags,
                               unsigned int& slot) override;
    
private:
    // Initialization
    // ----------------------------------------
    void InitEnvironmentFramebuffers(const unsigned int cubeSize);
    void InitEnvironmentMaterials();
    
    // Update
    // ----------------------------------------
    void UpdateEnvironment();
    void UpdateSphericalHarmonics();
    
    // Render
    // ----------------------------------------
    void RenderCubeMap(const std::array<glm::mat4, 6> &views, const glm::mat4 &projection,
                       const std::shared_ptr<Material> &material,
                       const std::shared_ptr<FrameBuffer> &framebuffer,
                       const unsigned int& viewportWidth = 0, const unsigned int& viewportHeight = 0,
                       const unsigned int &level = 0, const bool& genMipMaps = true);
    
    // Environment variables
    // ----------------------------------------
private:
    ///< The ambient light intensity.
    float m_AmbientStrength = 0.4f;
    ///< The environment map.
    std::shared_ptr<Texture> m_EnvironmentMap;
    ///< Spherical harmonics coefficients.
    SHCoefficients m_Coefficients;
    
    ///< Framebuffer(s) for pre-processing.
    FrameBufferLibrary m_Framebuffers;
    ///< Material(s) for pre-processing.
    MaterialLibrary m_Materials;
    
    /// Environment map orientation (pitch, yaw, and roll angles).
    glm::vec3 m_Rotation = glm::vec3(0.0f, -90.0f, 0.0f);
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(EnvironmentLight);
};
