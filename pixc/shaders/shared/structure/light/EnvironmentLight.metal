/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Represents a environment light in the scene.
 */
struct Environment
{
    int LightCount;                     ///< Number of active lights in the environment.
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
    
    //float La;                       ///< Ambient light intensity.
    //Light Lights[2];                ///< Array of lights (fixed max count)
};
