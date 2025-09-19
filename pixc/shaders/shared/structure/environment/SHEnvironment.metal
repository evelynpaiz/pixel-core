/// Maximum number of supported lights in the scene.
#define MAX_NUMBER_LIGHTS 4

/**
 * Stores precomputed spherical harmonic (SH) irradiance data for RGB channels.
 */
struct SHMatrix
{
    float4x4 Red;
    float4x4 Green;
    float4x4 Blue;
};

/**
 * Represents a environment light in the scene.
 */
struct Environment
{
    SHMatrix Irradiance;                ///< Spherical harmonic matrices
    float La;                           ///< Ambient light intensity.
    
    int LightCount;                     ///< Number of active lights in the environment.
    packed_float2 _pad0;                ///< Padding (align to 16 bytes)
    
    Light Lights[MAX_NUMBER_LIGHTS];    ///< Array of light sources affecting the scene.
};
