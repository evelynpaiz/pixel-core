#pragma once

#include "Common/Renderer/Texture/Texture.h"

/**
 * Represents a three-dimensional texture.
 *
 * The `Texture3D` class specializes the `Texture` base class to provide functionality for creating
 * and managing 3D textures. These textures consist of volumetric data, organized as a set of 2D slices.
 *
 * Like other texture types, `Texture3D` objects can be bound to specific texture units for use within
 * shaders.
 *
 * @note Copying or moving `Texture3D` objects is disabled to ensure single ownership and prevent
 * unintended resource duplication.
 */
class Texture3D : public Texture
{
public:
    // Constructor(s)
    // ----------------------------------------
    static std::shared_ptr<Texture3D> Create();
    static std::shared_ptr<Texture3D> Create(const TextureSpecification& spec);
    
    static std::shared_ptr<Texture3D> CreateFromData(const void *data);
    static std::shared_ptr<Texture3D> CreateFromData(const void *data,
                                                     const TextureSpecification& spec);
protected:
    // Constructor(s)
    // ----------------------------------------
    Texture3D() : Texture() {}
    Texture3D(const TextureSpecification& spec) : Texture(spec) {}
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Texture3D);
};

/**
 * Utility functions related to texture operations.
 */
namespace utils { namespace textures {

template <>
struct TextureHelper<Texture3D>
{
    /// @brief Sets the width, height and depth of a `TextureSpecification` for a `Texture3D`.
    /// @param spec The `TextureSpecification` object whose size needs to be set.
    /// @param size The width, height and depth to set for the texture.
    static void SetSize(TextureSpecification& spec, unsigned int size)
    {
        spec.SetTextureSize(size, size, size);
    }
};

/**
 * Get a shared pointer to a 1x1x1 white texture.
 *
 * @return A 1x1x1 white texture.
 */
DEFINE_WHITE_TEXTURE(Texture3D)

} // namespace Texturing
} // namespace utils
