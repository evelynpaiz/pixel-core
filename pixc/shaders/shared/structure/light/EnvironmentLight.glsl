/**
 * Represents a environment light in the scene.
 */
struct Environment
{
    float La;                       ///< Ambient light intensity.
    int LightsNumber;               ///< Number of lights in the environment.
    
    mat4 IrradianceMatrix[3];       ///< Spherical harmonic matrices for irradiance, [0] = red, [1] = green, [2] = blue.
};
