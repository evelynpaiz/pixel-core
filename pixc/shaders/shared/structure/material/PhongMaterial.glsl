/**
 * Represents the material properties of an object.
 */
struct Material
{
    uint Flags;                 ///< Shading flags.
    float Shininess;            ///< Shininess/exponent of the material for specular highlights.
    
    vec3 Ka;                    ///< Ambient reflectance/color of the material.
    vec3 Kd;                    ///< Diffuse reflectance/color of the material.
    vec3 Ks;                    ///< Specular reflectance/color of the material.
    
    float Alpha;                ///< Transparency/alpha value of the material.
    
    sampler2D DiffuseMap;       ///< Diffuse texture map of the material.
    sampler2D SpecularMap;      ///< Specular texture map of the material.
};
