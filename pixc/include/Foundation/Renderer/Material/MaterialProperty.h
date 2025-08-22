#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Flags describing configurable properties of a material.
 *
 * The `MaterialProperty` enum defines bitwise flags that describe common and optional
 * features or capabilities of materials in the rendering engine.
 *
 * These flags can be combined using bitwise operators to specify multiple properties simultaneously.
 */
enum class MaterialProperty : uint32_t
{
    None          = 0,        ///< No properties.
    ViewDirection = 1 << 0,   ///< Uses view direction in the shader.
    NormalMatrix  = 1 << 1    ///< Uses normal matrix in the shader.
};

/**
 * @brief Combine two `MaterialProperty` values using bitwise OR.
 *
 * Allows combining multiple `MaterialProperty` flags.
 *
 * @param a First set of properties.
 * @param b Second set of properties.
 *
 * @return Combined set of properties.
 */
inline MaterialProperty operator|(MaterialProperty a, MaterialProperty b)
{
    return static_cast<MaterialProperty>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

/**
 * @brief Compute the bitwise AND of two `MaterialProperty` values.
 *
 * Used to check which flags are set in common.
 *
 * @param a First set of properties.
 * @param b Second set of properties.
 *
 * @return Resulting flags after AND operation.
 */
inline MaterialProperty operator&(MaterialProperty a, MaterialProperty b)
{
    return static_cast<MaterialProperty>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

/**
 * @brief Compute the bitwise NOT of a `MaterialProperty` value.
 *
 * Useful for toggling or clearing specific flags.
 *
 * @param a Property flags.
 *
 * @return Bitwise negation of the input flags.
 */
inline MaterialProperty operator~(MaterialProperty a)
{
    return static_cast<MaterialProperty>(
        ~static_cast<uint32_t>(a)
    );
}

/**
 * @brief Bitwise OR assignment operator for `MaterialProperty`.
 *
 * Combines flags and assigns the result back to the left operand.
 *
 * @param a Reference to the first operand.
 * @param b The second operand.
 *
 * @return Reference to the modified first operand.
 */
inline MaterialProperty& operator|=(MaterialProperty& a, MaterialProperty b)
{
    a = a | b;
    return a;
}

/**
 * @brief Bitwise AND assignment operator for `MaterialProperty`.
 *
 * Performs bitwise AND and assigns the result back to the left operand.
 *
 * @param a Reference to the first operand.
 * @param b The second operand.
 *
 * @return Reference to the modified first operand.
 */
inline MaterialProperty& operator&=(MaterialProperty& a, MaterialProperty b)
{
    a = a & b;
    return a;
}

} // namespace pixc
