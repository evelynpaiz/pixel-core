/**
 * Represents the material properties of an object.
 */
struct Material {
    vec4 Color;            ///< Diffuse color of the material.
    sampler2D TextureMap;  ///< Texture map applied to the material.
};
