#pragma once

#include "Foundation/Renderer/Texture/Texture2D.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a two-dimensional texture for OpenGL.
 *
 * The `OpenGLTexture2D` class specializes the `Texture2D` class to provide
 * a concrete implementation using the OpenGL API. It handles the creation, binding,
 * unbinding, and resource management of two-dimensional texture data within an
 * OpenGL context.
 *
 * This class supports multisampling for enhanced visual quality.
 *
 * @note Copying and moving `OpenGLTexture2D` objects is disabled to prevent
 * unintended resource ownership issues.
 */
class OpenGLTexture2D : public Texture2D, public OpenGLTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLTexture2D(uint8_t samples);
    OpenGLTexture2D(const TextureSpecification& spec, uint8_t samples);
    
    
    OpenGLTexture2D(const void *data, uint8_t samples);
    OpenGLTexture2D(const void *data, const TextureSpecification& spec,
                    uint8_t samples);
    
    OpenGLTexture2D(const std::filesystem::path& filePath, bool flip);
    OpenGLTexture2D(const std::filesystem::path& filePath,
                    const TextureSpecification& spec, bool flip);
    
    DEFINE_OPENGL_TEXTURE_DESTRUCTOR(Texture2D)
    
    // Usage
    // ----------------------------------------
    DEFINE_OPENGL_TEXTURE_BINDING_METHODS()
    
private:
    DEFINE_OPENGL_TEXTURE_RESOURCE_METHODS()
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLTexture2D);
};

} // namespace pixc
