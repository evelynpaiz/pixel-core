/**
 * Represents a light source in the scene.
 */
struct Light {
    float4 Vector;   ///< Position of the light source in world space if .w is defined as 1.0f,
                     ///< Direction of the light source in world space if .w is defined as 0.0f
    
    float3 Color;    ///< Color/intensity of the light.
    
    float Ld;        ///< Diffuse light intensity.
    float Ls;        ///< Specular light intensity.
};
