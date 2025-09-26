/**
 * Represents the material properties of an object.
 */
struct Material
{
    packed_float3 Ka;               ///< Ambient reflectance/color
    float _pad0;                    ///< Padding to align next member

    packed_float3 Kd;               ///< Diffuse reflectance/color
    float _pad1;                    ///< Padding

    packed_float3 Ks;               ///< Specular reflectance/color
    float _pad2;                    ///< Padding

    float Shininess;                ///< Shininess/exponent
    float Alpha;                    ///< Transparency/alpha
    packed_float2 _pad3;            ///< Padding to make struct size a multiple of 16 bytes
};
