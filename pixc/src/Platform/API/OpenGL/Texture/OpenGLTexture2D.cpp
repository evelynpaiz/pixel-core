#include "enginepch.h"
#include "Platform/OpenGL/Texture/OpenGLTexture2D.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

// --------------------------------------------
// Texture (2D)
// --------------------------------------------

/**
 * Create a base 2D texture.
 */
OpenGLTexture2D::OpenGLTexture2D(uint8_t samples)
    : Texture2D(samples), OpenGLTexture()
{
    m_Spec.Type = samples > 1 ? TextureType::TEXTURE2D_MULTISAMPLE : TextureType::TEXTURE2D;
}

/**
 * Create a 2D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& spec,
                                 uint8_t samples)
    : Texture2D(spec, samples), OpenGLTexture()
{
    m_Spec.Type = samples > 1 ? TextureType::TEXTURE2D_MULTISAMPLE : TextureType::TEXTURE2D;
}

/**
 * Create a 2D texture from input data.
 *
 * @param data The data for the 2D texture.
 */
OpenGLTexture2D::OpenGLTexture2D(const void *data, uint8_t samples)
    : OpenGLTexture2D(samples)
{
    CreateTexture(data);
}

/**
 * Create a 2D texture from input data and with specific properties.
 *
 * @param data The data for the 2D texture.
 * @param spec The texture specifications.
 */
OpenGLTexture2D::OpenGLTexture2D(const void *data,
                                 const TextureSpecification& spec,
                                 uint8_t samples)
    : OpenGLTexture2D(spec, samples)
{
    CreateTexture(data);
}

/**
 * Create a 2D texture from the input source file.
 *
 * @param filePath Texture file path.
 * @param flip Fip the texture vertically.
 */
OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filePath,
                                 bool flip)
    : Texture2D(filePath, flip), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE2D;
    
    LoadFromFile(filePath);
}

/**
 * Create a 2D texture from the input source file.
 *
 * @param filePath Texture file path.
 * @param spec The texture specifications.
 * @param flip Fip the texture vertically.
 */
OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filePath,
                                 const TextureSpecification& spec, bool flip)
    : Texture2D(filePath, spec, flip), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURE2D;
    
    LoadFromFile(filePath);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void OpenGLTexture2D::CreateTexture(const void *data)
{
    // Verify size of the 2D texture
    CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0, "2D texture size not properly defined!");
    
    // Bind the texture
    Bind();
    
    // For multisample textures, use glTexImage2DMultisample to create the texture
    if (m_Samples > 1)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples,
                                utils::textures::gl::ToOpenGLInternalFormat(m_Spec.Format),
                                m_Spec.Width, m_Spec.Height, GL_FALSE);
        return;
    }
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, false));
    
    // Create the texture based on the format and data type
    if (utils::textures::IsDepthFormat(m_Spec.Format))
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                       m_Spec.Width, m_Spec.Height);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, utils::textures::gl::ToOpenGLInternalFormat(m_Spec.Format),
                     m_Spec.Width, m_Spec.Height, 0, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                     utils::textures::gl::ToOpenGLDataFormat(m_Spec.Format), data);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_2D);
    
    // Unbind the texture
    Unbind();
    
    // Define the texture as loaded
    m_IsLoaded = true;
}
