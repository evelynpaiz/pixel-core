#include "pixcpch.h"
#include "Foundation/Renderer/Texture/TextureCube.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/Utils/FactoryUtils.h"

#include "Platform/OpenGL/Texture/OpenGLTextureCube.h"
#ifdef __APPLE__
#include "Platform/Metal/Texture/MetalTextureCube.h"
#endif

#include <stb_image.h>

namespace pixc {

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::Create()
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube)
}

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::Create(const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, spec)
}

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @param data The data to be placed on all the faces of the cube.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromData(const void *data)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, data)
}

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @param data The data for the cube texture (defined for each face).
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromData(const std::vector<const void *>& data)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, data)
}

/**
 * @brief Create a 2D texture based on the active rendering API.
 *
 * @param data The data to be placed on all the faces of the cube.
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromData(const void *data,
                                                         const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, data, spec)
}

/**
 * @brief Create a 2D texture based on the active rendering API.
 *
 * @param data The data for the cube texture (defined for each face).
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromData(const std::vector<const void *>& data,
                                                         const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, data, spec)
}

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 * @param flip Fip the texture vertically.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromFile(const std::filesystem::path& directory,
                                                         const std::vector<std::string>& files, bool flip)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, directory, files, flip)
}

/**
 * @brief Create a cube texture based on the active rendering API.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 * @param flip Fip the texture vertically.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<TextureCube> TextureCube::CreateFromFile(const std::filesystem::path& directory,
                                                         const std::vector<std::string>& files,
                                                         const TextureSpecification& spec, bool flip)
{
    CREATE_RENDERER_OBJECT(std::make_shared, TextureCube, directory, files, spec, flip)
}

/**
 * @brief Load the texture from an input (image) source file.
 *
 * @param directory Textures file path.
 * @param files List of texture files.
 */
void TextureCube::LoadFromFile(const std::filesystem::path& directory,
                               const std::vector<std::string>& files)
{
    // Check that the data contains exactly 6 faces
    PIXEL_CORE_ASSERT(files.size() == 6, "Invalid data for the texture cube map!");
    
    // Load the image into our local buffer
    int width, height, channels;
    std::vector<const void*> data(files.size(), nullptr);
    
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
            PIXEL_CORE_WARN("Failed to load: " + filePath.filename().string());
            return;
        }
        
        // Save the corresponding image information
        Update(width, height, channels);
        PIXEL_CORE_ASSERT((unsigned int)m_Spec.Format, "Data format of " + filePath.filename().string() + " not supported!");
    }
    
    // Generate the cube texture
    CreateTexture(data);
    
    // Free memory
    for(unsigned int i = 0; i < data.size(); i++)
        stbi_image_free(const_cast<void*>(data[i]));
}

} // namespace pixc
