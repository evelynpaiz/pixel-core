#include "pixcpch.h"
#include "Platform/Metal/Texture/MetalTexture3D.h"

namespace pixc {

/**
 * @brief Create a base 3D texture.
 */
MetalTexture3D::MetalTexture3D()
    : Texture3D(), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURE3D;
}

/**
 * @brief Create a 3D base texture with specific properties.
 *
 * @param spec The texture specifications.
 */
MetalTexture3D::MetalTexture3D(const TextureSpecification& spec)
    : Texture3D(spec), MetalTexture()
{
    m_Spec.Type = TextureType::TEXTURE3D;
}

/**
 * @brief Create a 3D texture from input data.
 *
 * @param data The data for the 3D texture.
 */
MetalTexture3D::MetalTexture3D(const void *data)
    : MetalTexture3D()
{
    CreateTexture(data);
}

/**
 * @brief Create a 3D texture from input data and with specific properties.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 */
MetalTexture3D::MetalTexture3D(const void *data, const TextureSpecification& spec)
    : MetalTexture3D(spec)
{
    CreateTexture(data);
}

/**
 * @brief Create and configure the texture based on the texture specification and provided data.
 *
 * @param data The texture data. This can be nullptr if the texture is to be written.
 */
void MetalTexture3D::CreateTexture(const void *data)
{
    // Verify size of the 3D texture
    PIXEL_CORE_ASSERT(m_Spec.Width > 0 && m_Spec.Height > 0 && m_Spec.Depth > 0,
                "3D texture size not properly defined!");
    
    // Create the texture with its data and specifications
    MTLCreateTexture(data, m_Spec);
    
    // Define the texture as loaded
    m_IsLoaded = true;
}

} // namespace pixc
