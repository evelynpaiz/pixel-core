#pragma once

#include "Common/Renderer/Texture/Texture.h"

/**
 * Represents a one-dimensional texture.
 *
 * The `Texture1D` class specializes the `Texture` base class to provide functionality for creating
 * and managing 1D textures. These textures consist of a single line of texel data.
 *
 * Like other texture types, `Texture1D` objects can be bound to specific texture units for use within
 * shaders.
 *
 * @note Copying or moving `Texture1D` objects is disabled to ensure single ownership and prevent
 * unintended resource duplication.
 */
class Texture1D : public Texture
{
public:
    // Constructor(s)
    // ----------------------------------------
    static std::shared_ptr<Texture1D> Create();
    static std::shared_ptr<Texture1D> Create(const TextureSpecification& spec);
    
    static std::shared_ptr<Texture1D> CreateFromData(const void *data);
    static std::shared_ptr<Texture1D> CreateFromData(const void *data,
                                                     const TextureSpecification& spec);
    
protected:
    // Constructor(s)
    // ----------------------------------------
    Texture1D() : Texture() {}
    Texture1D(const TextureSpecification& spec) : Texture(spec) {}
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Texture1D);
};

/**
 * Utility functions related to texture operations.
 */
namespace utils { namespace textures {

template <>
struct TextureHelper<Texture1D>
{
    /// @brief Sets the width of a `TextureSpecification` for a `Texture1D`.
    /// @param spec The `TextureSpecification` object whose size needs to be set.
    /// @param size The width to set for the texture.
    static void SetSize(TextureSpecification& spec, unsigned int size)
    {
        spec.SetTextureSize(size);
    }
};

/**
 * Get a shared pointer to a 1x white texture.
 *
 * @return A 1x white texture.
 */
DEFINE_WHITE_TEXTURE(Texture1D)

} // namespace Texturing
} // namespace utils
