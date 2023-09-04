/**
 * Represents a light source in the scene.
 */
struct Light {
    vec4 Vector;    ///< Position of the light source in world space if .w is defined as 1.0f,
                    ///< Direction of the light source in world space if .w is defined as 0.0f
    
    vec3 Color;      ///< Color/intensity of the light.
    
    float La;        ///< Ambient light intensity.
    float Ld;        ///< Diffuse light intensity.
    float Ls;        ///< Specular light intensity.
};
