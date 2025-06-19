#include "enginepch.h"
#include "Platform/OpenGL/Texture/OpenGLTexture.h"

#include "Platform/OpenGL/Texture/OpenGLTexture1D.h"
#include "Platform/OpenGL/Texture/OpenGLTexture2D.h"
#include "Platform/OpenGL/Texture/OpenGLTexture3D.h"
#include "Platform/OpenGL/Texture/OpenGLTextureCube.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

/**
 * Create an OpenGL texture.
 */
OpenGLTexture::OpenGLTexture()
{
    GLCreate();
}

/**
 * Generate an new OpenGL texture.
 */
void OpenGLTexture::GLCreate()
{
    glGenTextures(1, &m_ID);
}

/**
 * Deletes an OpenGL texture.
 */
void OpenGLTexture::GLRelease()
{
    glDeleteTextures(1, &m_ID);
}

/**
 * Binds a texture to the active texture unit.
 *
 * @param ID   The ID of the texture object to bind.
 * @param type The type of the texture (e.g., `TextureType::TEXTURE2D`).
 */
void OpenGLTexture::GLBind(TextureType type) const
{
    glBindTexture(utils::textures::gl::ToOpenGLTextureTarget(type), m_ID);
}

/**
 * Binds a texture to a specific texture unit.
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
 * Unbinds the texture from the active texture unit.
 *
 * @param type The type of the texture to unbind.
 */
void OpenGLTexture::GLUnbind(TextureType type) const
{
    glBindTexture(utils::textures::gl::ToOpenGLTextureTarget(type), 0);
}

/**
 * Retrieves the OpenGL texture ID from a Texture object.
 *
 * @param attachment A shared pointer to the `Texture` object.
 *
 * @return The OpenGL texture ID (`m_ID`) if the cast is successful and the texture type is supported. 
 */
uint32_t OpenGLTexture::GLGetTextureID(const std::shared_ptr<Texture>& texture)
{
    auto& spec = texture->GetSpecification();
    switch (spec.Type)
    {
        case TextureType::None:
            break;
        case TextureType::TEXTURE1D:
        {
            if (auto tex = std::dynamic_pointer_cast<OpenGLTexture1D>(texture); tex)
                return tex->m_ID;
            break;
        }
        case TextureType::TEXTURE2D:
        case TextureType::TEXTURE2D_MULTISAMPLE:
        {
            if (auto tex = std::dynamic_pointer_cast<OpenGLTexture2D>(texture); tex)
                return tex->m_ID;
            break;
        }
        case TextureType::TEXTURE3D: {
            if (auto tex = std::dynamic_pointer_cast<OpenGLTexture3D>(texture); tex)
                return tex->m_ID;
            break;
        }
        case TextureType::TEXTURECUBE: {
            if (auto tex = std::dynamic_pointer_cast<OpenGLTextureCube>(texture); tex)
                return tex->m_ID;
            break;
        }
    }
    
    CORE_ASSERT(false, "Unsupported texture type encountered!");
    return 0;
}
