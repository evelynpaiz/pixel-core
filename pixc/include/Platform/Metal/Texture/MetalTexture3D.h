#pragma once

#include "Foundation/Renderer/Texture/Texture3D.h"
#include "Platform/Metal/Texture/MetalTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a three-dimensional texture for Metal.
 *
 * The `MetalGLTexture3D` class specializes the `Texture3D` class to provide a concrete
 * implementation using the Metal API. It handles the creation, binding, unbinding, and resource
 * management of one-dimensional texture data within a Metal context.
 *
 * @note Copying and moving `MetalTexture3D` objects is disabled o prevent unintended
 * resource ownership issues.
 */
class MetalTexture3D : public Texture3D, public MetalTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalTexture3D();
    MetalTexture3D(const TextureSpecification& spec);
    
    MetalTexture3D(const void *data);
    MetalTexture3D(const void *data, const TextureSpecification& spec);
    
    DEFINE_MTL_TEXTURE_DESTRUCTOR(Texture3D)
    
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
    DISABLE_COPY_AND_MOVE(MetalTexture3D);
};

} // namespace pixc
