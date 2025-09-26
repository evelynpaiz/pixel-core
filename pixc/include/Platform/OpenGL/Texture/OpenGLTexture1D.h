#pragma once

#include "Foundation/Renderer/Texture/Texture1D.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a one-dimensional texture for OpenGL.
 *
 * The `OpenGLTexture1D` class specializes the `Texture1D` class to provide a concrete
 * implementation using the OpenGL API. It handles the creation, binding, unbinding, and resource
 * management of one-dimensional texture data within an OpenGL context.
 *
 * @note Copying and moving `OpenGLTexture1D` objects is disabled o prevent unintended
 * resource ownership issues.
 */
class OpenGLTexture1D : public Texture1D, public OpenGLTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLTexture1D();
    OpenGLTexture1D(const TextureSpecification& spec);
    
    OpenGLTexture1D(const void *data);
    OpenGLTexture1D(const void *data, const TextureSpecification& spec);
    
    DEFINE_OPENGL_TEXTURE_DESTRUCTOR(Texture1D)
    
    // Usage
    // ----------------------------------------
    DEFINE_OPENGL_TEXTURE_BINDING_METHODS()
    
protected:
    DEFINE_OPENGL_TEXTURE_RESOURCE_METHODS()
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLTexture1D);
};

} // namespace pixc
