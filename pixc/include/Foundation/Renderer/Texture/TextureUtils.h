#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Enumeration representing the types of textures.
 *
 * The `TextureType` enum class defines the various types of textures that can be used
 * within a graphics application. It provides identifiers for different texture categories,
 * which are used for different rendering purposes.
 * This enum is typically used in functions and structures to specify the type of a texture.
 */
enum class TextureType
{
    None = 0,               ///< No texture.
    TEXTURE1D,              ///< One-dimensional texture.
    TEXTURE2D,              ///< Standard two-dimensional texture.
    TEXTURE2D_MULTISAMPLE,  ///< Multi-sampling two-dimensional texture.
    TEXTURE3D,              ///< Three-dimensional texture data.
    TEXTURECUBE,            ///< Six-sided cube map texture.
};

/**
 * @brief Enumeration of internal texture formats used for specifying pixel formats in textures.
 *
 * The `TextureFormat` enum provides a list of supported internal formats for textures. These
 * formats determine how pixel data is stored in the graphics memory. Each format has different
 * bit-depths and arrangements of color channels.
 */
enum class TextureFormat
{
    None = 0,           ///< No format.
    
    // Color formats
    R8,                 ///< 8-bit single channel (red only).
    RG8,                ///< 8-bit per channel, 16-bit total (two channels).
    RGB8,               ///< 8-bit per channel, 24-bit total (no alpha).
    RGBA8,              ///< 8-bit per channel, 32-bit total (standard color texture).
    
    R16F,               ///< 16-bit half-float single channel (HDR, red channel only)
    RG16F,              ///< 16-bit half-float per channel, 32-bit total (HDR, red and blue channels only)
    RGB16F,             ///< 16-bit half-float per channel, 48-bit total (HDR, no alpha)
    RGBA16F,            ///< 16-bit half-float per channel, 64-bit total (HDR color texture)
    
    R32F,               ///< 32-bit float single channel (HDR, red channel only)
    RG32F,              ///< 32-bit half-float per channel, 64-bit total (HDR, red and blue channels only)
    RGB32F,             ///< 32-bit float per channel, 96-bit total (HDR, no alpha)
    RGBA32F,            ///< 32-bit float per channel, 128-bit total (HDR color texture)
    
    // Integer formats
    R8UI,               ///< 8-bit unsigned integer (usually used for IDs).
    RG8UI,              ///< 8-bit unsigned integer per channel (two channels, IDs).
    RGB8UI,             ///< 8-bit unsigned integer per channel (three channels, IDs).
    RGBA8UI,            ///< 8-bit unsigned integer per channel (four channels, IDs).
    
    // Depth/stencil formats
    DEPTH16,             ///< 8-bit depth (depth buffer).
    DEPTH24,             ///< 24-bit depth (depth buffer).
    DEPTH32,             ///< 32-bit depth (depth buffer).
    DEPTH32F,            ///< 32-bit float depth (depth buffer).
    DEPTH24STENCIL8,     ///< 24-bit depth, 8-bit stencil (depth and stencil attachments).
};

/**
 * @brief Enumeration of different texture wrapping modes.
 *
 * The `TextureWrap` enum provides a list of supported texture wrapping modes that determine
 * how texture coordinates outside the [0, 1] range are handled. These modes dictate how the
 * texture is repeated or clamped when texture coordinates extend beyond the original texture size.
 */
enum class TextureWrap
{
    None = 0,
    Repeat,             ///< Repeat the texture.
    MirroredRepeat,     ///< Repeat the texture with mirroring.
    ClampToEdge,        ///< Clamp the texture coordinates to the edge of the texture.
    ClampToBorder,      ///< Clamp the texture coordinates to a border color.
};

/**
 * @brief Enumeration of texture filtering modes.
 *
 * The `TextureFilter` enum provides a list of supported texture filtering modes that
 * determine how textures are sampled when their original resolution differs from the rendering
 * resolution. These modes control how the GPU decides which texels to use for sampling and
 * rendering.
 */
enum class TextureFilter
{
    None = 0,
    Nearest,            ///< Nearest-neighbor filtering (nearest pixel).
    Linear,             ///< Linear filtering (interpolate between neighboring pixels).
};

/**
 * @brief Enumeration of shader binding indices for textures.
 */
enum class TextureIndex : uint32_t
{
    EnvironmentMap  = 0,    ///< Environment cubemap (used for IBL/specular reflections)
    TextureMap      = 1,    ///< Main texture (albedo)
    DiffuseMap      = 1,    ///< Optional diffuse map (may overlap with TextureMap)
    SpecularMap     = 2,    ///< Specular map (controls intensity / roughness)
    
    ShadowMap0      = 5,    ///< Shadow map for light 0
    ShadowMap1      = 6,    ///< Shadow map for light 1
    ShadowMap2      = 7,    ///< Shadow map for light 2
    ShadowMap3      = 8     ///< Shadow map for light 3
};

/**
 * @namespace utils::textures
 * @brief Utility functions related to texture operations.
 */
namespace utils { namespace textures {

/**
 * @brief Define the number of channels in the texture based on its format.
 *
 * @param format The texture format.
 *
 * @return Channels number.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline unsigned int GetChannelCount(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
        case TextureFormat::R8UI: return 1;
            
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
        case TextureFormat::RG8UI: return 2;
            
        case TextureFormat::RGB32F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB8:
        case TextureFormat::RGB8UI: return 3;
            
        case TextureFormat::RGBA32F:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA8UI: return 4;
            
        case TextureFormat::None:
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH24STENCIL8: return 0;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return 0;
}

/**
 * @brief Define the number of bytes (per channel) in the texture based on its format.
 *
 * @param format The texture format.
 *
 * @return Bytes (per pixel) number.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline unsigned int GetBytesPerChannel(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None:
            return 0;
            
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
            
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:
            return 1;  // Each channel (R, G, B) is 1 byte
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            return 2; // Each channel (R, G, B) is 2 bytes
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
            return 4; // Each channel (R, G, B) is 4 bytes
            
        case TextureFormat::DEPTH16:
            return 2;
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH24STENCIL8:
            return 3;
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:
            return 4;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return 0;
}

/**
 * @brief Verify if a texture format can be represented as depth format.
 *
 * @param format The texture format.
 *
 * @return Wheter the format is a depth format.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline bool IsDepthFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH24STENCIL8:
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH16: return true;
            
        case TextureFormat::None:
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
            
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI: return false;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return false;
}

/**
 * @brief Verify if a texture format can be represented as RGB format.
 *
 * @param format The texture format.
 *
 * @return Wheter the format is a RGB format.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline bool IsRGBFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGB8UI:     return true;
            
        case TextureFormat::None:
        case TextureFormat::R8:
        case TextureFormat::RG8:
            
        case TextureFormat::RGBA8:
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
            
        case TextureFormat::RGBA16F:
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
            
        case TextureFormat::RGBA32F:
            
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGBA8UI:
            
        case TextureFormat::DEPTH24STENCIL8:
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH16:    return false;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return false;
}

/**
 * @brief Allocate memory for a buffer based on the specified texture format and size.
 *
 * @param format The texture format for which the buffer is allocated.
 * @param bufferSize The size of the buffer to be allocated.
 *
 * @return A void pointer to the allocated buffer. Returns nullptr if the format is not recognized.
 *
 * @note Ensure to delete[] the allocated memory after use to avoid memory leaks.
 */
inline void* AllocateTextureData(TextureFormat format, unsigned int size)
{
    switch (format)
    {
        case TextureFormat::None: return nullptr;
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI: return static_cast<void*>(new char[size]);
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24STENCIL8: return static_cast<void*>(new int[size]);
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::DEPTH32F: return static_cast<void*>(new float[size]);
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown texture format!");
    return nullptr;
}

/**
 * @brief Deallocate memory for a buffer based on the specified texture format.
 *
 * @param format The texture format for which the buffer was allocated.
 * @param buffer A pointer to the allocated buffer.
 *
 * @note Ensure to use this function to delete the buffer to avoid memory leaks.
 */
inline void FreeTextureData(TextureFormat format, void* buffer)
{
    switch (format)
    {
        case TextureFormat::None:
            // Do nothing, as nullptr was returned in AllocateBufferForFormat
            break;
            
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:
            delete[] static_cast<char*>(buffer);
            break;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24STENCIL8:
            delete[] static_cast<int*>(buffer);
            break;
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::DEPTH32F:
            delete[] static_cast<float*>(buffer);
            break;
            
        default:
            PIXEL_CORE_ASSERT(false, "Unknown texture format!");
            break;
    }
}

} // namespace textures
} // namespace utils
} // namespace pixc
