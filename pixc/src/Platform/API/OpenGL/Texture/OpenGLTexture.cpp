#include "pixcpch.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

namespace pixc {

/**
 * @brief Create an OpenGL texture.
 */
OpenGLTexture::OpenGLTexture()
{
    GLCreate();
}

/**
 * @brief Generate an new OpenGL texture.
 */
void OpenGLTexture::GLCreate()
{
    glGenTextures(1, &m_ID);
}

/**
 * @brief Deletes an OpenGL texture.
 */
void OpenGLTexture::GLRelease()
{
    glDeleteTextures(1, &m_ID);
}

/**
 * @brief Binds a texture to the active texture unit.
 *
 * @param ID   The ID of the texture object to bind.
 * @param type The type of the texture (e.g., `TextureType::TEXTURE2D`).
 */
void OpenGLTexture::GLBind(TextureType type) const
{
    glBindTexture(utils::textures::gl::ToOpenGLTextureTarget(type), m_ID);
}

/**
 * @brief Binds a texture to a specific texture unit.
 *
 * @param ID   The ID of the texture object to bind.
 * @param type The type of the texture.
 * @param slot The index of the texture unit to bind to.
 */
void OpenGLTexture::GLBindToTextureUnit(TextureType type, uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    GLBind(type);
}

/**
 * @brief Unbinds the texture from the active texture unit.
 *
 * @param type The type of the texture to unbind.
 */
void OpenGLTexture::GLUnbind(TextureType type) const
{
    glBindTexture(utils::textures::gl::ToOpenGLTextureTarget(type), 0);
}

} // namespace pixc
