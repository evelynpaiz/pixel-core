#include "pixcpch.h"
#include "Platform/OpenGL/Texture/OpenGLTexture1D.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

namespace pixc {

/**
 * @brief Create a base 1D texture.
 */
OpenGLTexture1D::OpenGLTexture1D()
: Texture1D(), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE1D;
}

/**
 * @brief Create a 1D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
OpenGLTexture1D::OpenGLTexture1D(const TextureSpecification& spec)
: Texture1D(spec), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE1D;
}

/**
 * @brief Create a 1D texture from input data.
 *
 * @param data The data for the 1D texture.
 */
OpenGLTexture1D::OpenGLTexture1D(const void *data)
: OpenGLTexture1D()
{
    CreateTexture(data);
}

/**
 * @brief Create a 1D texture from input data and with specific properties.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 */
OpenGLTexture1D::OpenGLTexture1D(const void *data, const TextureSpecification& spec)
: OpenGLTexture1D(spec)
{
    CreateTexture(data);
}

/**
 * @brief Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void OpenGLTexture1D::CreateTexture(const void *data)
{
    // Verify size of the 1D texture
    PIXEL_CORE_ASSERT(m_Spec.Width > 0, "1D texture size not properly defined!");
    
    // Bind the texture
    Bind();
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, false));
    
    // Create the texture based on the format and data type
    if (utils::textures::IsDepthFormat(m_Spec.Format))
    {
        glTexStorage1D(GL_TEXTURE_1D, 1, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                       m_Spec.Width);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexImage1D(GL_TEXTURE_1D, 0, utils::textures::gl::ToOpenGLInternalFormat(m_Spec.Format),
                     m_Spec.Width, 0, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                     utils::textures::gl::ToOpenGLDataFormat(m_Spec.Format), data);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_1D);
    
    // Unbind the texture
    Unbind();
    
    // Define the texture as loaded
    m_IsLoaded = true;
}

} // namespace pixc
