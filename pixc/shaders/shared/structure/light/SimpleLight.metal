/**
 * Represents a light source in the scene.
 */
struct Light
{
    packed_float4 Vector;   ///< Position of the light source in world space if .w is defined as 1.0f,
                            ///< Direction of the light source in world space if .w is defined as 0.0f
    
    packed_float3 Color;    ///< Color/intensity of the light.
    float _pad0;            ///< Padding (align to 16 bytes)
    
    float Ld;               ///< Diffuse light intensity.
    float Ls;               ///< Specular light intensity.
    packed_float2 _pad1;    ///< Padding (align to 16 bytes)
};
