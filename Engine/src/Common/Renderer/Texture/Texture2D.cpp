#include "enginepch.h"
#include "Common/Renderer/Texture/Texture2D.h"

#include <GL/glew.h>
#include <stb_image.h>

// --------------------------------------------
// Texture (2D)
// --------------------------------------------

/**
 * Create a base 2D texture.
 */
Texture2D::Texture2D(const int& samples)
    : Texture(), m_Samples(samples)
{
    m_Spec.Type = TextureType::TEXTURE2D;
}

/**
 * Create a 2D texture from input data.
 *
 * @param data The data for the 2D texture.
 */
Texture2D::Texture2D(const void *data, const int& samples)
    : Texture2D(samples)
{
    CreateTexture(data);
}

/**
 * Create a 2D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
Texture2D::Texture2D(const TextureSpecification& spec, const int& samples)
    : Texture(spec), m_Samples(samples)
{
    m_Spec.Type = TextureType::TEXTURE2D;
}

/**
 * Create a 2D texture from input data and with specific properties.
 *
 * @param data The data for the 2D texture.
 * @param spec The texture specifications.
 */
Texture2D::Texture2D(const void *data, const TextureSpecification& spec, const int& samples)
    : Texture2D(spec, samples)
{
    CreateTexture(data);
}

/**
 * Get the texture target based on the texture specification.
 *
 * @return The OpenGL texture target.
 */
GLenum Texture2D::TextureTarget() const
{
    return (GLenum)(m_Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void Texture2D::CreateTexture(const void *data)
{
    // Verify size of the 2D texture
    CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0,
                "2D texture size not properly defined!");
    
    // Bind the texture
    Bind();
    
    // For multisample textures, use glTexImage2DMultisample to create the texture
    if (m_Samples > 1)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples,
                                utils::OpenGL::TextureFormatToOpenGLInternalType(m_Spec.Format),
                                m_Spec.Width, m_Spec.Height, GL_FALSE);
        return;
    }
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    utils::OpenGL::TextureWrapToOpenGLType(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    utils::OpenGL::TextureWrapToOpenGLType(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    utils::OpenGL::TextureFilterToOpenGLType(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    utils::OpenGL::TextureFilterToOpenGLType(m_Spec.Filter, false));
    
    // Create the texture based on the format and data type
    if (utils::OpenGL::IsDepthFormat(m_Spec.Format))
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, utils::OpenGL::TextureFormatToOpenGLBaseType(m_Spec.Format),
                       m_Spec.Width, m_Spec.Height);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, utils::OpenGL::TextureFormatToOpenGLInternalType(m_Spec.Format),
                     m_Spec.Width, m_Spec.Height, 0, utils::OpenGL::TextureFormatToOpenGLBaseType(m_Spec.Format),
                     utils::OpenGL::TextureFormatToOpenGLDataType(m_Spec.Format), data);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_2D);
    
    // Unbind the texture
    Unbind();
}

// --------------------------------------------
// Texture Resource (2D)
// --------------------------------------------

/**
 * Generate a texture from the input source file.
 *
 * @param filePath Texture file path.
 * @param flip Fip the texture vertically.
 */
Texture2DResource::Texture2DResource(const std::filesystem::path& filePath, bool flip)
    : Texture2D(), m_FilePath(filePath), m_Flip(flip)
{
    LoadFromFile(filePath);
}

/**
 * Load the texture from an input (image) source file.
 *
 * @param filePath Texture file path.
 */
void Texture2DResource::LoadFromFile(const std::filesystem::path& filePath)
{
    // Determine whether to flip the image vertically
    stbi_set_flip_vertically_on_load(m_Flip);
    
    // Extract the file extension
    std::string extension = filePath.extension().string();
    
    // Load the image into the local buffer
    int width, height, channels;
    void* data = nullptr;
    
    data = (extension != ".hdr") ? stbi_load(filePath.string().c_str(), &width, &height, &channels, 0) :
                            (void*)stbi_loadf(filePath.string().c_str(), &width, &height, &channels, 0);
    
    // Verify that the image has been loaded correctly
    if (!data)
    {
        CORE_WARN("Failed to load: " + filePath.filename().string());
        return;
    }
    
    // Save the corresponding image information
    utils::Texturing::UpdateSpecsTextureResource(m_Spec, width, height, channels, extension);
    CORE_ASSERT((unsigned int)m_Spec.Format, "Data format of " + m_FilePath.filename().string() + " not supported!");
    
    // Generate the 2D texture
    CreateTexture(data);
    
    // Free memory
    stbi_image_free(data);
}
