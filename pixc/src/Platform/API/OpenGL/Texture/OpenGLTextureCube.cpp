#include "pixcpch.h"
#include "Platform/OpenGL/Texture/OpenGLTextureCube.h"

#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <GL/glew.h>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Create a cube texture with no data defined.
 */
OpenGLTextureCube::OpenGLTextureCube()
: TextureCube(), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * @brief Create a cube texture with specific properties and no data defined.
 *
 * @param spec The texture specifications.
 */
OpenGLTextureCube::OpenGLTextureCube(const TextureSpecification& spec)
: TextureCube(spec), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * @brief Create a cube texture from input data.
 *
 * @param data The data to be placed on all the faces of the cube.
 */
OpenGLTextureCube::OpenGLTextureCube(const void *data)
: OpenGLTextureCube()
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data.
 *
 * @param data The data for the cube texture (defined for each face).
 */
OpenGLTextureCube::OpenGLTextureCube(const std::vector<const void *>& data)
: OpenGLTextureCube()
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data and with specific properties.
 *
 * @param data The data to be placed on all the faces of the cube.
 * @param spec The texture specifications.
 */
OpenGLTextureCube::OpenGLTextureCube(const void *data,
                                     const TextureSpecification& spec)
: OpenGLTextureCube(spec)
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data and with specific properties.
 *
 * @param data The data for the cube texture (defined for each face).
 * @param spec The texture specifications.
 */
OpenGLTextureCube::OpenGLTextureCube(const std::vector<const void *>& data,
                                     const TextureSpecification& spec)
: OpenGLTextureCube(spec)
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from the input source files.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 * @param flip Fip the texture vertically.
 */
OpenGLTextureCube::OpenGLTextureCube(const std::filesystem::path& directory,
                                     const std::vector<std::string>& files, bool flip)
: TextureCube(directory, files, flip), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
    
    LoadFromFile(directory, files);
}

/**
 * @brief Create a cube texture from the input source files.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 * @param spec The texture specifications.
 * @param flip Fip the texture vertically.
 */
OpenGLTextureCube::OpenGLTextureCube(const std::filesystem::path& directory,
                                     const std::vector<std::string>& files,
                                     const TextureSpecification& spec,
                                     bool flip)
: TextureCube(directory, files, spec, flip), OpenGLTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
    
    LoadFromFile(directory, files);
}

/**
 * @brief Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The data to be placed on all the faces of the cube.
 */
void OpenGLTextureCube::CreateTexture(const void *data)
{
    // The data is duplicated for all faces of the cube map
    std::vector<const void *> cube(6, data);
    CreateTexture(cube);
}

/**
 * @brief Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The data for the cube texture (defined for each face).
 */
void OpenGLTextureCube::CreateTexture(const std::vector<const void *> &data)
{
    // Check that the data contains exactly 6 faces
    PIXEL_CORE_ASSERT(data.size() == 6, "Invalid data for the texture cube map!");
    
    // Bind the texture
    Bind();
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                    utils::textures::gl::ToOpenGLWrap(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    utils::textures::gl::ToOpenGLFilter(m_Spec.Filter, false));
    
    // Verify size of the 2D texture
    PIXEL_CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0, "2D texture size not properly defined!");
    
    for (unsigned int i = 0; i < data.size(); ++i)
    {
        // Create the texture with the data
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, utils::textures::gl::ToOpenGLInternalFormat(m_Spec.Format),
                     m_Spec.Width, m_Spec.Height, 0, utils::textures::gl::ToOpenGLBaseFormat(m_Spec.Format),
                     utils::textures::gl::ToOpenGLDataFormat(m_Spec.Format), data[i]);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // Unbind the texture
    Unbind();
    
    // Define the texture as loaded
    m_IsLoaded = true;
}

} // namespace pixc
