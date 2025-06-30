#include "pixcpch.h"
#include "Platform/Metal/Texture/MetalTextureCube.h"

namespace pixc {

/**
 * @brief Create a cube texture with no data defined.
 */
MetalTextureCube::MetalTextureCube()
: TextureCube(), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * @brief Create a cube texture with specific properties and no data defined.
 *
 * @param spec The texture specifications.
 */
MetalTextureCube::MetalTextureCube(const TextureSpecification& spec)
: TextureCube(spec), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
}

/**
 * @brief Create a cube texture from input data.
 *
 * @param data The data to be placed on all the faces of the cube.
 */
MetalTextureCube::MetalTextureCube(const void *data)
: MetalTextureCube()
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data.
 *
 * @param data The data for the cube texture (defined for each face).
 */
MetalTextureCube::MetalTextureCube(const std::vector<const void *>& data)
: MetalTextureCube()
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data and with specific properties.
 *
 * @param data The data to be placed on all the faces of the cube.
 * @param spec The texture specifications.
 */
MetalTextureCube::MetalTextureCube(const void *data,
                                   const TextureSpecification& spec)
: MetalTextureCube(spec)
{
    CreateTexture(data);
}

/**
 * @brief Create a cube texture from input data and with specific properties.
 *
 * @param data The data for the cube texture (defined for each face).
 * @param spec The texture specifications.
 */
MetalTextureCube::MetalTextureCube(const std::vector<const void *>& data,
                                   const TextureSpecification& spec)
: MetalTextureCube(spec)
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
MetalTextureCube::MetalTextureCube(const std::filesystem::path& directory,
                                   const std::vector<std::string>& files, bool flip)
: TextureCube(directory, files, flip), MetalTexture()
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
MetalTextureCube::MetalTextureCube(const std::filesystem::path& directory,
                                   const std::vector<std::string>& files,
                                   const TextureSpecification& spec,
                                   bool flip)
: TextureCube(directory, files, spec, flip), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURECUBE;
    
    LoadFromFile(directory, files);
}

/**
 * @brief Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The data to be placed on all the faces of the cube.
 */
void MetalTextureCube::CreateTexture(const void *data)
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
void MetalTextureCube::CreateTexture(const std::vector<const void *> &data)
{
    // Check that the data contains exactly 6 faces
    PIXEL_CORE_ASSERT(data.size() == 6, "Invalid data for the texture cube map!");
    
    // Create the texture with its data and specifications
    for (unsigned int i = 0; i < data.size(); ++i)
        MTLCreateTexture(data[0], m_Spec, 1, i);
    
    // Define the texture as loaded
    m_IsLoaded = true;
}

} // namespace pixc
