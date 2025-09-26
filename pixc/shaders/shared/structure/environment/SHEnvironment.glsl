/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Stores precomputed spherical harmonic (SH) irradiance data for RGB channels.
 */
struct SHMatrix
{
    mat4 Red;
    mat4 Green;
    mat4 Blue;
};

/**
 * Represents the environment in the scene.
 */
struct Environment
{
    samplerCube EnvironmentMap;         ///< Cube map texture containing the environment surrounding the scene.
    
    SHMatrix Irradiance;                ///< Spherical harmonic matrices for irradiance.
    float La;                           ///< Ambient light intensity.
    
    int LightCount;                     ///< Number of active lights in the environment.
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
};
