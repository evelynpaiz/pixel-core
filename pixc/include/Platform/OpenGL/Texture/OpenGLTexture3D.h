#pragma once

#include "Foundation/Renderer/Texture/Texture3D.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of a three-dimensional texture for OpenGL.
 *
 * The `OpenGLTexture3D` class specializes the `Texture3D` class to provide a concrete
 * implementation using the OpenGL API. It handles the creation, binding, unbinding, and resource
 * management of one-dimensional texture data within an OpenGL context.
 *
 * @note Copying and moving `OpenGLTexture3D` objects is disabled o prevent unintended
 * resource ownership issues.
 */
class OpenGLTexture3D : public Texture3D, public OpenGLTexture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLTexture3D();
    OpenGLTexture3D(const TextureSpecification& spec);
    
    OpenGLTexture3D(const void *data);
    OpenGLTexture3D(const void *data, const TextureSpecification& spec);
    
    DEFINE_OPENGL_TEXTURE_DESTRUCTOR(Texture3D)
    
    // Usage
    // ----------------------------------------
    DEFINE_OPENGL_TEXTURE_BINDING_METHODS()
    
protected:
    DEFINE_OPENGL_TEXTURE_RESOURCE_METHODS()
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLTexture3D);
};

} // namespace pixc
