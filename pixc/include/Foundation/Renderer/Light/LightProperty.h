#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Flags describing configurable properties of a light source.
 *
 * The `LightProperty` enum defines bitwise flags that describe common and optional features of
 * lights in the rendering engine.
 *
 * These flags can be combined using bitwise operators to specify multiple properties simultaneously.
 */
enum class LightProperty : uint32_t
{
    None              = 0,        ///< No properties.
    
    GeneralProperties = 1 << 0,   ///< General light attributes (color, position/direction).
    
    DiffuseLighting   = 1 << 1,   ///< Diffuse lighting enabled.
    SpecularLighting  = 1 << 2,   ///< Specular lighting enabled.
    
    ShadowProperties  = 1 << 3    ///< Shadow mapping enabled.
};

/**
 * @brief Combine two `LightProperty` values using bitwise OR.
 *
 * Allows combining multiple `LightProperty` flags.
 *
 * @param a First set of properties.
 * @param b Second set of properties.
 *
 * @return Combined set of properties.
 */
inline LightProperty operator|(LightProperty a, LightProperty b)
{
    return static_cast<LightProperty>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

/**
 * @brief Compute the bitwise AND of two `LightProperty` values.
 *
 * Used to check which flags are set in common.
 *
 * @param a First set of properties.
 * @param b Second set of properties.
 *
 * @return Resulting flags after AND operation.
 */
inline LightProperty operator&(LightProperty a, LightProperty b)
{
    return static_cast<LightProperty>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

/**
 * @brief Compute the bitwise NOT of a `LightProperty` value.
 *
 * Useful for toggling or clearing specific flags.
 *
 * @param a Property flags.
 *
 * @return Bitwise negation of the input flags.
 */
inline LightProperty operator~(LightProperty a)
{
    return static_cast<LightProperty>(
        ~static_cast<uint32_t>(a)
    );
}

/**
 * @brief Bitwise OR assignment operator for `LightProperty`.
 *
 * Combines flags and assigns the result back to the left operand.
 *
 * @param a Reference to the first operand.
 * @param b The second operand.
 *
 * @return Reference to the modified first operand.
 */
inline LightProperty& operator|=(LightProperty& a, LightProperty b)
{
    a = a | b;
    return a;
}

/**
 * @brief Bitwise AND assignment operator for `LightProperty`.
 *
 * Performs bitwise AND and assigns the result back to the left operand.
 *
 * @param a Reference to the first operand.
 * @param b The second operand.
 *
 * @return Reference to the modified first operand.
 */
inline LightProperty& operator&=(LightProperty& a, LightProperty b)
{
    a = a & b;
    return a;
}

} // namespace pixc
