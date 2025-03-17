#include "enginepch.h"
#include "Common/Renderer/Texture/Texture2D.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Texture/OpenGLTexture2D.h"
#include "Platform/Metal/Texture/MetalTexture2D.h"

#include <stb_image.h>

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param samples The number of samples to use for multisampling.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture2D> Texture2D::Create(uint8_t samples)
{
    CREATE_RENDERER_OBJECT(Texture2D, samples)
}

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param spec The texture specifications.
 * @param samples The number of samples to use for multisampling.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture2D> Texture2D::Create(const TextureSpecification& spec,
                                             uint8_t samples)
{
    CREATE_RENDERER_OBJECT(Texture2D, spec, samples)
}

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param data The data for the 2D texture.
 * @param samples The number of samples to use for multisampling.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture2D> Texture2D::CreateFromData(const void *data, 
                                                     uint8_t samples)
{
    CREATE_RENDERER_OBJECT(Texture2D, data, samples)
}

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 * @param samples The number of samples to use for multisampling.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture2D> Texture2D::CreateFromData(const void *data,
                                                     const TextureSpecification& spec,
                                                     uint8_t samples)
{
    CREATE_RENDERER_OBJECT(Texture2D, data, spec, samples)
}

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param filePath Texture file path.
 * @param flip Fip the texture vertically.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture2D> Texture2D::CreateFromFile(const std::filesystem::path& filePath,
                                                     bool flip)
{
    CREATE_RENDERER_OBJECT(Texture2D, filePath, flip)
}

/**
 * Create a 2D texture based on the active rendering API.
 *
 * @param filePath Texture file path.
 * @param spec The texture specifications.
 * @param flip Fip the texture vertically.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */

std::shared_ptr<Texture2D> Texture2D::CreateFromFile(const std::filesystem::path& filePath,
                                                     const TextureSpecification& spec, bool flip)
{
    CREATE_RENDERER_OBJECT(Texture2D, filePath, spec, flip)
}

/**
 * Load the texture from an input (image) source file.
 *
 * @param filePath Texture file path.
 */
void Texture2D::LoadFromFile(const std::filesystem::path& filePath)
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
    Update(width, height, channels, extension);
    CORE_ASSERT((unsigned int)m_Spec.Format, "Data format of " + m_Path.filename().string() + " not supported!");
    
    // Generate the 2D texture
    CreateTexture(data);
    
    // Free memory
    stbi_image_free(data);
}
