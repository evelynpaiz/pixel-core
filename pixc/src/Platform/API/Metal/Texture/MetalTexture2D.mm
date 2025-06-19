#include "enginepch.h"
#include "Platform/Metal/Texture/MetalTexture2D.h"

/**
 * Create a base 2D texture.
 */
MetalTexture2D::MetalTexture2D(uint8_t samples)
    : Texture2D(samples), MetalTexture()
{
    m_Spec.Type = samples > 1 ? TextureType::TEXTURE2D_MULTISAMPLE : TextureType::TEXTURE2D;
}

/**
 * Create a 2D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
MetalTexture2D::MetalTexture2D(const TextureSpecification& spec,
                                 uint8_t samples)
    : Texture2D(spec, samples), MetalTexture()
{
    m_Spec.Type = samples > 1 ? TextureType::TEXTURE2D_MULTISAMPLE : TextureType::TEXTURE2D;
}

/**
 * Create a 2D texture from input data.
 *
 * @param data The data for the 2D texture.
 */
MetalTexture2D::MetalTexture2D(const void *data, uint8_t samples)
    : MetalTexture2D(samples)
{
    CreateTexture(data);
}

/**
 * Create a 2D texture from input data and with specific properties.
 *
 * @param data The data for the 2D texture.
 * @param spec The texture specifications.
 */
MetalTexture2D::MetalTexture2D(const void *data,
                               const TextureSpecification& spec,
                               uint8_t samples)
    : MetalTexture2D(spec, samples)
{
    CreateTexture(data);
}

/**
 * Create a 2D texture from the input source file.
 *
 * @param filePath Texture file path.
 * @param flip Fip the texture vertically.
 */
MetalTexture2D::MetalTexture2D(const std::filesystem::path& filePath,
                               bool flip)
    : Texture2D(filePath, flip), MetalTexture()
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
MetalTexture2D::MetalTexture2D(const std::filesystem::path& filePath,
                               const TextureSpecification& spec, bool flip)
    : Texture2D(filePath, spec, flip), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURE2D;
    
    LoadFromFile(filePath);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void MetalTexture2D::CreateTexture(const void *data)
{
    // Verify if the size of the texture has been properly defined
    CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0, "2D texture size not properly defined!");
    
    // Create the texture with its data and specifications
    MTLCreateTexture(data, m_Spec, m_Samples);
    
    // Define the texture as loaded
    m_IsLoaded = true;
}
