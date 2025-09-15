/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Represents a environment light in the scene.
 */
struct Environment
{
    int LightCount;                     ///< Number of active lights in the environment.
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
    
    float La;                           ///< Ambient light intensity.
    //mat4 IrradianceMatrix[3];         ///< Spherical harmonic matrices for irradiance, [0] = red, [1] = green, [2] = blue.
};
