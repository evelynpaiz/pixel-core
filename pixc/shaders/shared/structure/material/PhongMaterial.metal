/**
 * Represents the material properties of an object.
 */
struct Material
{
    float Shininess;              ///< Shininess/exponent of the material for specular highlights.
    float _pad0[3];               ///< Padding to align Ka to 16 bytes

    packed_float3 Ka;             ///< Ambient reflectance/color of the material.
    float _pad1;                  ///< Padding

    packed_float3 Kd;             ///< Diffuse reflectance/color of the material.
    float _pad2;                  ///< Padding

    packed_float3 Ks;             ///< Specular reflectance/color of the material.
    float Alpha;                  ///< Transparency/alpha value
};
