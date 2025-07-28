/**
 * Represents the material properties of an object.
 */
struct Material {
    sampler2D DiffuseMap;    ///< Diffuse texture map of the material.
    sampler2D SpecularMap;   ///< Specular texture map of the material.
    
    float Shininess;         ///< Shininess factor for specular highlights.
    float Alpha;             ///< Alpha transparency value of the material.
};
