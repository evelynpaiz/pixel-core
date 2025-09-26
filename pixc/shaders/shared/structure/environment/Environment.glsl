/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Represents the environment in the scene.
 */
struct Environment
{
    float La;                           ///< Ambient light intensity.
    
    int LightCount;                     ///< Number of active lights in the environment.
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
};
