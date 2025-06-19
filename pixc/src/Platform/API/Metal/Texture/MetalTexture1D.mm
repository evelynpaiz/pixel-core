#include "enginepch.h"
#include "Platform/Metal/Texture/MetalTexture1D.h"

/**
 * Create a base 1D texture.
 */
MetalTexture1D::MetalTexture1D()
    : Texture1D(), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURE1D;
}

/**
 * Create a 1D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
MetalTexture1D::MetalTexture1D(const TextureSpecification& spec)
    : Texture1D(spec), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURE1D;
}

/**
 * Create a 1D texture from input data.
 *
 * @param data The data for the 1D texture.
 */
MetalTexture1D::MetalTexture1D(const void *data)
    : MetalTexture1D()
{
    CreateTexture(data);
}

/**
 * Create a 1D texture from input data and with specific properties.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 */
MetalTexture1D::MetalTexture1D(const void *data, const TextureSpecification& spec)
    : MetalTexture1D(spec)
{
    CreateTexture(data);
}

/**
 * Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void MetalTexture1D::CreateTexture(const void *data)
{
    // Verify size of the 1D texture
    CORE_ASSERT(m_Spec.Width > 0, "1D texture size not properly defined!");
     
    // Create the texture with its data and specifications
    MTLCreateTexture(data, m_Spec);
    
    // Define the texture as loaded
    m_IsLoaded = true;
}
