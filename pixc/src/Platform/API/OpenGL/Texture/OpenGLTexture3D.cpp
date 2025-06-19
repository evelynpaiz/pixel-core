#include "enginepch.h"
#include "Platform/OpenGL/Texture/OpenGLTexture3D.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

/**
 * Create a base 3D texture.
 */
OpenGLTexture3D::OpenGLTexture3D()
    : Texture3D(), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE3D;
}

/**
 * Create a 3D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
OpenGLTexture3D::OpenGLTexture3D(const TextureSpecification& spec)
    : Texture3D(spec), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE3D;
}

/**
 * Create a 3D texture from input data.
 *
 * @param data The data for the 3D texture.
 */
OpenGLTexture3D::OpenGLTexture3D(const void *data)
    : OpenGLTexture3D()
{
    CreateTexture(data);
}

/**
 * Create a 3D texture from input data and with specific properties.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 */
OpenGLTexture3D::OpenGLTexture3D(const void *data, const TextureSpecification& spec)
    : OpenGLTexture3D(spec)
{
    CreateTexture(data);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void OpenGLTexture3D::CreateTexture(const void *data)
{
    // Verify size of the 3D texture
    CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0 && m_Spec.Depth > 0,
                "3D texture size not properly defined!");
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, false));
    
    // Create the texture based on the format and data type
    if (utils::textures::IsDepthFormat(m_Spec.Format))
    {
        glTexStorage3D(GL_TEXTURE_3D, 1, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                       m_Spec.Width, m_Spec.Height, m_Spec.Depth);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexImage3D(GL_TEXTURE_3D, 0, utils::textures::gl::ToOpenGLInternalFormat(m_Spec.Format),
                     m_Spec.Width, m_Spec.Height, m_Spec.Depth, 0,
                     utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                     utils::textures::gl::ToOpenGLDataFormat(m_Spec.Format), data);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_3D);
    
    // Unbind the texture
    Unbind();
    
    // Define the texture as loaded
    m_IsLoaded = true;
}
