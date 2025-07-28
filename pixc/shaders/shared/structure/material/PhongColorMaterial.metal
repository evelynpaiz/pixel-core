/**
 * Represents the material properties of an object.
 */
struct Material {
    float3 Ka;              ///< Ambient reflectance/color of the material.
    float3 Kd;              ///< Diffuse reflectance/color of the material.
    float3 Ks;              ///< Specular reflectance/color of the material.
    
    float Shininess;        ///< Shininess/exponent of the material for specular highlights.
    float Alpha;            ///< Transparency/alpha value of the material.
};
