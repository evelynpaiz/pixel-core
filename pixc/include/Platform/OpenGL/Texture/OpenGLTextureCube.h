#pragma once

#include "Foundation/Renderer/Texture/TextureCube.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a cubemap texture for OpenGL.
 *
 * The `OpenGLTextureCube` class specializes the `TextureCube` class to provide
 * a concrete implementation using the OpenGL API. It handles the creation, binding,
 * unbinding, and resource management of cubemap texture data within an
 * OpenGL context.
 *
 * @note Copying and moving `OpenGLTextureCube` objects is disabled to prevent
 * unintended resource ownership issues.
 */
class OpenGLTextureCube : public TextureCube, public OpenGLTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLTextureCube();
    OpenGLTextureCube(const TextureSpecification& spec);
    
    OpenGLTextureCube(const void *data);
    OpenGLTextureCube(const std::vector<const void *>& data);
    OpenGLTextureCube(const void *data, const TextureSpecification& spec);
    OpenGLTextureCube(const std::vector<const void *>& data,
                      const TextureSpecification& spec);
    
    OpenGLTextureCube(const std::filesystem::path& directory,
                      const std::vector<std::string>& files, bool flip);
    OpenGLTextureCube(const std::filesystem::path& directory,
                      const std::vector<std::string>& files,
                      const TextureSpecification& spec,
                      bool flip);
    
    DEFINE_OPENGL_TEXTURE_DESTRUCTOR(TextureCube)
    
    // Usage
    // ----------------------------------------
    DEFINE_OPENGL_TEXTURE_BINDING_METHODS()
    
protected:
    DEFINE_OPENGL_TEXTURE_RESOURCE_METHODS()
    void CreateTexture(const std::vector<const void *> &data) override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLTextureCube);
};

} // namespace pixc
