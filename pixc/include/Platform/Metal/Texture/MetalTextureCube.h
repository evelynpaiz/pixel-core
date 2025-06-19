#pragma once

#include "Common/Renderer/Texture/TextureCube.h"
#include "Platform/Metal/Texture/MetalTexture.h"

/**
 * Concrete implementation of a cubemap texture for Metal.
 *
 * The `MetalTextureCube` class specializes the `TextureCube` class to provide
 * a concrete implementation using the Metal API. It handles the creation, binding,
 * unbinding, and resource management of cubemap texture data within an
 * Metal context.
 *
 * @note Copying and moving `MetalTextureCube` objects is disabled to prevent
 * unintended resource ownership issues.
 */
class MetalTextureCube : public TextureCube, MetalTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalTextureCube();
    MetalTextureCube(const TextureSpecification& spec);
    
    MetalTextureCube(const void *data);
    MetalTextureCube(const std::vector<const void *>& data);
    MetalTextureCube(const void *data, const TextureSpecification& spec);
    MetalTextureCube(const std::vector<const void *>& data,
                     const TextureSpecification& spec);
    
    MetalTextureCube(const std::filesystem::path& directory,
                     const std::vector<std::string>& files, bool flip);
    MetalTextureCube(const std::filesystem::path& directory,
                     const std::vector<std::string>& files,
                     const TextureSpecification& spec,
                     bool flip);
    
    DEFINE_MTL_TEXTURE_DESTRUCTOR(TextureCube)
    
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
    void CreateTexture(const std::vector<const void *> &data) override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalTextureCube);
};
