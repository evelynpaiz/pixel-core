/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Represents a environment light in the scene.
 */
struct Environment
{
    float La;                           ///< Ambient light intensity.
    
    int LightCount;                     ///< Number of active lights in the environment.
    packed_float2 _pad0;                ///< Padding (align to 16 bytes)
    
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
};
