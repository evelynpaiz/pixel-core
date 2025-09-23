#pragma once

#include "Foundation/Renderer/Light/Environment/EnvironmentLight.h"
#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents spherical harmonic (SH) coefficients for isotropic and anisotropic irradiance.
 */
class SHCoefficients
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Default constructor.
    SHCoefficients() = default;
    /// @brief @brief Constructs SH coefficient matrices from the given coefficients.
    ///  @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
    ///                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
    SHCoefficients(const std::vector<float>& shCoeffs) { UpdateFromCoefficients(shCoeffs); }
    
    /// @brief Default destructor.
    ~SHCoefficients() = default;
    
    // Initialization
    // ----------------------------------------
    void UpdateFromCoefficients(const std::vector<float>& shCoeffs);
    
private:
    // Matrix Computation
    // ----------------------------------------
    static glm::mat4 GenerateIsotropicMatrix(int colorIndex,
                                             const std::vector<float>& shCoeffs);
    static glm::mat4 GenerateAnisotropicMatrix(int colorIndex,
                                               const std::vector<float>& shCoeffs);
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class SHEnvironmentLight;
    
private:
    // Spherical Harmonics variables
    // ----------------------------------------
    /**
     * A structure that stores precomputed spherical harmonic (SH) irradiance data for RGB channels.
     */
    struct SHMatrix
    {
        glm::mat4 Red = glm::mat4(0.0f);    ///< 4x4 matrix for the red channel.
        glm::mat4 Green = glm::mat4(0.0f);  ///< 4x4 matrix for the green channel.
        glm::mat4 Blue = glm::mat4(0.0f);   ///< 4x4 matrix for the blue channel.
    };
    
    SHMatrix m_Isotropic;                   ///< SH coefficients for isotropic irradiance (same in all directions).
    SHMatrix m_Anisotropic;                 ///< SH coefficients for anisotropic irradiance (direction-dependent).
};

class SHEnvironmentLight : public EnvironmentLight
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    SHEnvironmentLight(const uint32_t size = 2048);
    
    /// @brief Destructor for the environment light.
    ~SHEnvironmentLight() override = default;
    
private:
    // Initialization
    // ----------------------------------------
    void SetupFramebuffers(const uint32_t size) override;
    void SetupResources() override;
    
    // Properties
    // ----------------------------------------
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               LightProperty properties) override;
    
    // Update
    // ----------------------------------------
    void UpdateEnvironment() override;
    
    // Environment variables
    // ----------------------------------------
private:
    ///< Spherical harmonics coefficients.
    SHCoefficients m_Coefficients;
};

} // namespace pixc
