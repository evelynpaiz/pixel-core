#include "pixcpch.h"
#include "Foundation/Renderer/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace pixc {

/**
 * @brief Update the specifications of a texture resource based on width, height, channels, and extension.
 *
 * @param width The width of the texture.
 * @param height The height of the texture.
 * @param channels The number of color channels in the texture.
 * @param extension The file extension (e.g., ".hdr") to determine if the texture is HDR.
 */
void Texture::Update(const unsigned int width, const unsigned int height,
                     const unsigned int channels, const std::string& extension)
{
    // Update width and height
    m_Spec.Width = width;
    m_Spec.Height = height;
    
    // Determine if the texture is HDR based on the file extension
    bool isHDR = (extension == ".hdr");
    
    // Define the format of the data to be used
    if (!isHDR && channels == 4)
        m_Spec.Format = TextureFormat::RGBA8;
    else if (!isHDR && channels == 3)
        m_Spec.Format = TextureFormat::RGB8;
    else if (isHDR && channels == 3)
        m_Spec.Format = TextureFormat::RGB32F;
    else
    {
        PIXEL_CORE_WARN("Trying to load a texture not supported!");
        m_Spec.Format = TextureFormat::None;
    }
        
    
    // Set default wrap mode if needed
    m_Spec.Wrap = (m_Spec.Wrap != TextureWrap::None) ? m_Spec.Wrap :
                  (isHDR ? TextureWrap::ClampToEdge : TextureWrap::Repeat);
    
    // Set default filter mode if needed
    m_Spec.Filter.Min = (m_Spec.Filter.Min != TextureFilter::None) ? m_Spec.Filter.Min : TextureFilter::Linear;
    m_Spec.Filter.Mag = (m_Spec.Filter.Mag != TextureFilter::None) ? m_Spec.Filter.Mag : TextureFilter::Linear;
    
    m_Spec.Filter.Mip = (m_Spec.MipMaps && m_Spec.Filter.Mip != TextureFilter::None) ?
                         m_Spec.Filter.Mip : TextureFilter::Linear;
}

/**
 * @brief Retrieves the number of channels in the texture.
 *
 * @returns The number of channels.
 */
int Texture::GetChannels() const
{
    // Get the number of channels in the texture
    int channels = utils::textures::GetChannelCount(m_Spec.Format);
    PIXEL_CORE_ASSERT(channels >= 1 && channels <= 4, "Invalid number of channels in the attachment!");
    
    // TODO: Verify support for depth attachments (channels = 0 for now)
    return channels;
}

/**
 * @brief Retrieves the number of channels, ensuring a 4-byte aligned stride.
 *
 * @return The adjusted number of channels ensuring a 4-byte aligned stride.
 */
int Texture::GetAlignedChannels() const
{
    int channels = GetChannels();
    int stride = GetStride();
    
    // Align stride to a 4-byte boundary if necessary
    channels += (stride % 4) ? (4 - stride % 4) : 0;
    return channels;
}

/**
 * @brief Computes the stride (row size in bytes) of the texture.
 *
 * @returns The computed stride of the texture in bytes.
 */
int Texture::GetStride() const
{
    // Calculate the stride size based on the format and width
    int bytes = utils::textures::GetBytesPerChannel(m_Spec.Format);     // Bytes per pixel
    int stride = bytes * GetChannels() * m_Spec.Width;                  // Bytes per row
    
    return stride;
}

} // namespace pixc
