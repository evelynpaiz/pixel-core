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
    
    ShadowProperties  = 1 << 3,   ///< Shadow mapping enabled.
    
    DirectionDependent  = 1 << 4  ///< Lighting depends on surface orientation or tangent space.
                                  ///< If not set, lighting is isotropic or normal-based.
};

// Enable bitwise operators for `LightProperty`.
ENABLE_ENUM_BITMASK(LightProperty);

} // namespace pixc
