#include "enginepch.h"
#include "Common/Renderer/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/**
 * Update the specifications of a texture resource based on width, height, channels, and extension.
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
    m_Spec.Format = TextureFormat::None;
    if (!isHDR && channels == 4)
        m_Spec.Format = TextureFormat::RGBA8;
    else if (!isHDR && channels == 3)
        m_Spec.Format = TextureFormat::RGB8;
    else if (isHDR && channels == 3)
        m_Spec.Format = TextureFormat::RGB16F;
    
    // Set default wrap mode based on HDR status
    if (m_Spec.Wrap == TextureWrap::None)
        m_Spec.Wrap = isHDR ? TextureWrap::ClampToEdge : TextureWrap::Repeat;
    
    // Set default filter mode
    if (m_Spec.Filter == TextureFilter::None)
        m_Spec.Filter = TextureFilter::Linear;
    
    // Enable mipmaps by default
    m_Spec.MipMaps = true;
}

/**
 * Retrieves the number of channels in the texture.
 *
 * @returns The number of channels.
 */
int Texture::GetChannels() const
{
    // Get the number of channels in the texture
    int channels = utils::textures::GetChannelCount(m_Spec.Format);
    CORE_ASSERT(channels >= 1 && channels <= 4, "Invalid number of channels in the attachment!");
    
    // TODO: Verify support for depth attachments (channels = 0 for now)
    return channels;
}

/**
 * Retrieves the number of channels, ensuring a 4-byte aligned stride.
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
 * Computes the stride (row size in bytes) of the texture.
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
