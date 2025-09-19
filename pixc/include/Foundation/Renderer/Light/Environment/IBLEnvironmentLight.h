#pragma once

#include "Foundation/Renderer/Light/Environment/EnvironmentLight.h"
#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class IBLEvironmentLight : public EnvironmentLight
{
    
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
