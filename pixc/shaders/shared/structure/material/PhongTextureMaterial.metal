/**
 * Represents the material properties of an object.
 */
struct Material
{
    float Shininess;                ///< Shininess/exponent
    float Alpha;                    ///< Transparency/alpha
    packed_float2 _pad3;            ///< Padding to make struct size a multiple of 16 bytes
};
