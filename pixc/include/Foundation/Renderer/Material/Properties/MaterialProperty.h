#pragma once

#include "Foundation/Renderer/Utils/BitMaskUtils.h"

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

// Enable bitwise operators for `MaterialProperty`.
ENABLE_ENUM_BITMASK(MaterialProperty);

} // namespace pixc
