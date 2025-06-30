#pragma once

#include "Foundation/Renderer/Texture/Texture1D.h"
#include "Platform/Metal/Texture/MetalTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a one-dimensional texture for Metal.
 *
 * The `MetalTexture1D` class specializes the `Texture1D` class to provide a concrete
 *  implementation using Metal. It handles the creation, binding, unbinding, and resource
 * management of one-dimensional texture data within a Metal context.
 *
 * @note Copying and moving `MetalTexture1D` objects is disabled o prevent unintended
 * resource ownership issues.
 */
class MetalTexture1D : public Texture1D, public MetalTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalTexture1D();
    MetalTexture1D(const TextureSpecification& spec);
    
    MetalTexture1D(const void *data);
    MetalTexture1D(const void *data, const TextureSpecification& spec);
    
    DEFINE_MTL_TEXTURE_DESTRUCTOR(Texture1D)
    
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
    DISABLE_COPY_AND_MOVE(MetalTexture1D);
};

} // namespace pixc
