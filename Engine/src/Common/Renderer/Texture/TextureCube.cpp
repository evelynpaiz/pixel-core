#include "enginepch.h"
#include "Common/Renderer/Texture/TextureCube.h"

#include <GL/glew.h>
#include <stb_image.h>

// --------------------------------------------
// Texture (3D)
// --------------------------------------------

/**
 * Create a base cube texture.
 */
TextureCube::TextureCube()
    : Texture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * Create a cube texture from input data.
 *
 * @param data The data for the cube texture.
 */
TextureCube::TextureCube(const void *data)
    : TextureCube()
{
    CreateTexture(data);
}

/**
 * Create a cube texture from input data.
 *
 * @param data The data for the cube texture.
 */
TextureCube::TextureCube(const std::vector<const void *>& data)
    : TextureCube()
{
    CreateTexture(data);
}

/**
 * Create a base cube texture with specific properties.
 *
 * @param spec The texture specifications.
 */
TextureCube::TextureCube(const TextureSpecification& spec)
    : Texture(spec), m_CubeSpecs(std::vector<TextureSpecification>(6, spec))
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * Create a cube texture from input data and with specific properties.
 *
 * @param data The data for the cube texture.
 * @param spec The texture specifications.
 */
TextureCube::TextureCube(const void *data, const TextureSpecification& spec)
    : TextureCube(spec)
{
    CreateTexture(data);
}

/**
 * Create a cube texture from input data and with specific properties.
 *
 * @param data The data for the cube texture.
 * @param spec The texture specifications.
 */
TextureCube::TextureCube(const std::vector<const void *>& data, const TextureSpecification& spec)
    : TextureCube(spec)
{
    CreateTexture(data);
}

/**
 * Get the texture target based on the texture specification.
 *
 * @return The OpenGL texture target.
 */
GLenum TextureCube::TextureTarget() const
{
    return (GLenum)GL_TEXTURE_CUBE_MAP;
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void TextureCube::CreateTexture(const void *data)
{
    // The data is duplicated for all faces of the cube map
    std::vector<const void *> cube(6, data);
    CreateTexture(cube);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void TextureCube::CreateTexture(const std::vector<const void *>& data)
{
    // Check that the data contains exactly 6 faces
    CORE_ASSERT(data.size() == 6, "Invalid data for the texture cube map!");
    
    // Bind the texture
    Bind();
    
    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                    utils::OpenGL::TextureWrapToOpenGLType(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                    utils::OpenGL::TextureWrapToOpenGLType(m_Spec.Wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                    utils::OpenGL::TextureWrapToOpenGLType(m_Spec.Wrap));
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    utils::OpenGL::TextureFilterToOpenGLType(m_Spec.Filter, m_Spec.MipMaps));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                    utils::OpenGL::TextureFilterToOpenGLType(m_Spec.Filter, false));
    
    for (unsigned int i = 0; i < data.size(); ++i)
    {
        // Verify size of the 2D texture
        CORE_ASSERT(m_CubeSpecs[i].Width > 0 && m_CubeSpecs[i].Height > 0,
                    "2D texture size not properly defined!");
        // Create the texture with the data
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, utils::OpenGL::TextureFormatToOpenGLInternalType(m_CubeSpecs[i].Format),
                     m_CubeSpecs[i].Width, m_CubeSpecs[i].Height, 0, utils::OpenGL::TextureFormatToOpenGLBaseType(m_CubeSpecs[i].Format),
                     utils::OpenGL::TextureFormatToOpenGLDataType(m_CubeSpecs[i].Format), data[i]);
    }
    
    // Generate mipmaps if specified
    if (m_Spec.MipMaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // Unbind the texture
    Unbind();
}

// --------------------------------------------
// Texture Cube Resource
// --------------------------------------------

/**
 * Generate a texture from the input source file.
 *
 * @param filePath Texture file path.
 * @param flip Fip the texture vertically.
 */
TextureCubeResource::TextureCubeResource(const std::filesystem::path& directory,
                                         const std::vector<std::string>& files, bool flip)
    : TextureCube(), m_Directory(directory), m_Files(files), m_Flip(flip)
{
    LoadFromFile(directory, files);
}

/**
 * Load the texture from an input (image) source file.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 */
void TextureCubeResource::LoadFromFile(const std::filesystem::path& directory,
                                       const std::vector<std::string>& files)
{
    // Check that the data contains exactly 6 faces
    CORE_ASSERT(files.size() == 6, "Invalid data for the texture cube map!");
    
    // Load the image into our local buffer
    int width, height, channels;
    std::vector<const void*> data(files.size(), nullptr);
    m_CubeSpecs = std::vector<TextureSpecification>(files.size(), TextureSpecification());
    
    for(unsigned int i = 0; i < data.size(); i++)
    {
        // Determine whether to flip the image vertically
        stbi_set_flip_vertically_on_load(m_Flip);
        
        // Define the file path
        std::filesystem::path filePath = directory / files[i];
        
        // Extract the file extension
        std::string extension = filePath.extension().string();
        
        data[i] = (extension != ".hdr") ? stbi_load(filePath.string().c_str(), &width, &height, &channels, 0) :
                                   (void*)stbi_loadf(filePath.string().c_str(), &width, &height, &channels, 0);
        
        // Verify that the image has been loaded correctly
        if (!data[i])
        {
            CORE_WARN("Failed to load: " + filePath.filename().string());
            return;
        }
        
        // Save the corresponding image information
        utils::Texturing::UpdateSpecsTextureResource(m_CubeSpecs[i], width, height, channels);
        CORE_ASSERT((unsigned int)m_CubeSpecs[i].Format,
                    "Data format of " + filePath.filename().string() + " not supported!");
    }
    // Get the general image information
    utils::Texturing::UpdateSpecsTextureResource(m_Spec, 0, 0, 0);
    
    // Generate the cube texture
    CreateTexture(data);
    
    // Free memory
    for(unsigned int i = 0; i < data.size(); i++)
        stbi_image_free(const_cast<void*>(data[i]));
}
