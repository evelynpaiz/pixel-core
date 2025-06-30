#pragma once

#include "Foundation/Renderer/Texture/Texture2D.h"
#include "Platform/Metal/Texture/MetalTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a two-dimensional texture for Metal.
 *
 * The `MetalTexture2D` class specializes the `Texture2D` class to provide
 * a concrete implementation using the Metal API. It handles the creation, binding,
 * unbinding, and resource management of two-dimensional texture data within a
 * Metal context.
 *
 * This class supports multisampling for enhanced visual quality.
 *
 * @note Copying and moving `MetalTexture2D` objects is disabled to prevent
 * unintended resource ownership issues.
 */
class MetalTexture2D : public Texture2D, public MetalTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalTexture2D(uint8_t samples);
    MetalTexture2D(const TextureSpecification& spec, uint8_t samples);
    
    
    MetalTexture2D(const void *data, uint8_t samples);
    MetalTexture2D(const void *data, const TextureSpecification& spec,
                   uint8_t samples);
    
    MetalTexture2D(const std::filesystem::path& filePath, bool flip);
    MetalTexture2D(const std::filesystem::path& filePath,
                   const TextureSpecification& spec, bool flip);
    
    DEFINE_MTL_TEXTURE_DESTRUCTOR(Texture2D)
    
    // Usage
    // ----------------------------------------
    DEFINE_MTL_TEXTURE_BINDING_METHODS()
    
    // Getter(s)
    // ----------------------------------------
    DEFINE_MTL_TEXTURE_GETTERS_METHODS()
    
protected:
    // Creation & Release
    // ----------------------------------------
    DEFINE_MTL_TEXTURE_RESOURCE_METHODS()
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalTexture2D);
};

} // namespace pixc
