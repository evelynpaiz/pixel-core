#pragma once

#include <GL/glew.h>

/**
 * Enumeration of internal texture formats used for specifying pixel formats in textures.
 *
 * The `TextureFormat` enum provides a list of supported internal formats for textures. These
 * formats determine how pixel data is stored in the graphics memory. Each format has different
 * bit-depths and arrangements of color channels.
 */
enum class TextureFormat
{
    None = 0,
    // Color formats
    R8,                 ///< 8-bit single channel (red only)
    RG8,                ///< 8-bit per channel, 16-bit total (two channels)
    RGB8,               ///< 8-bit per channel, 24-bit total (no alpha)
    RGBA8,              ///< 8-bit per channel, 32-bit total (standard color texture)
    
    R16F,               ///< 16-bit half-float single channel (HDR, red channel only)
    RG16F,              ///< 16-bit half-float per channel, 32-bit total (HDR, red and blue channels only)
    RGB16F,             ///< 16-bit half-float per channel, 48-bit total (HDR, no alpha)
    RGBA16F,            ///< 16-bit half-float per channel, 64-bit total (HDR color texture)
    
    R32F,               ///< 32-bit float single channel (HDR, red channel only)
    RG32F,              ///< 32-bit half-float per channel, 64-bit total (HDR, red and blue channels only)
    RGB32F,             ///< 32-bit float per channel, 96-bit total (HDR, no alpha)
    RGBA32F,            ///< 32-bit float per channel, 128-bit total (HDR color texture)
    
    // Integer formats
    R8UI,               ///< 8-bit unsigned integer (usually used for IDs)
    RG8UI,              ///< 8-bit unsigned integer per channel (two channels, IDs)
    RGB8UI,             ///< 8-bit unsigned integer per channel (three channels, IDs)
    RGBA8UI,            ///< 8-bit unsigned integer per channel (four channels, IDs)
    
    // Depth/stencil formats
    DEPTH16,             ///< 8-bit depth (depth buffer)
    DEPTH24,             ///< 24-bit depth (depth buffer)
    DEPTH32,             ///< 32-bit depth (depth buffer)
    DEPTH32F,            ///< 32-bit float depth (depth buffer)
    DEPTH24STENCIL8,     ///< 24-bit depth, 8-bit stencil (depth and stencil attachments)
    
    // Defaults
    Depth = DEPTH24
};

/**
 * Enumeration of different texture wrapping modes.
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
 * Enumeration of texture filtering modes.
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
 * Utility functions related to texture operations.
 */
namespace utils {

/// @brief Namespace containing utility functions for OpenGL conversions.
namespace OpenGL
{
/**
 * Convert the texture format to its corresponding OpenGL (base) format type.
 *
 * @param format The texture format.
 *
 * @return OpenGL texture (base) format type.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline GLenum TextureFormatToOpenGLBaseType(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None: return 0;
        case TextureFormat::R8:
        case TextureFormat::R16F: 
        case TextureFormat::R32F: return GL_RED;
        case TextureFormat::RG8: 
        case TextureFormat::RG16F: 
        case TextureFormat::RG32F: return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F: return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F: return GL_RGBA;
            
        case TextureFormat::R8UI: return GL_RED_INTEGER;
        case TextureFormat::RG8UI: return GL_RG_INTEGER;
        case TextureFormat::RGB8UI: return GL_RGB_INTEGER;
        case TextureFormat::RGBA8UI: return GL_RGBA_INTEGER;
            
        case TextureFormat::DEPTH16: return GL_DEPTH_COMPONENT16;
        case TextureFormat::DEPTH24: return GL_DEPTH_COMPONENT24;
        case TextureFormat::DEPTH32: return GL_DEPTH_COMPONENT32;
        case TextureFormat::DEPTH32F: return GL_DEPTH_COMPONENT32F;
        case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
    }
    
    CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

/**
 * Convert the texture format to its corresponding OpenGL (internal) format type.
 *
 * @param format The texture format.
 *
 * @return OpenGL texture (internal) format type.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline GLenum TextureFormatToOpenGLInternalType(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None: return 0;
        case TextureFormat::R8: return GL_R8;
        case TextureFormat::RG8: return GL_RG8;
        case TextureFormat::RGB8: return GL_RGB8;
        case TextureFormat::RGBA8: return GL_RGBA16;
            
        case TextureFormat::R16F: return GL_R16F;
        case TextureFormat::RG16F: return GL_RG16F;
        case TextureFormat::RGB16F: return GL_RGB16F;
        case TextureFormat::RGBA16F: return GL_RGBA16F;
            
        case TextureFormat::R32F: return GL_R32F;
        case TextureFormat::RG32F: return GL_RG32F;
        case TextureFormat::RGB32F: return GL_RGB32F;
        case TextureFormat::RGBA32F: return GL_RGBA32F;
            
        case TextureFormat::R8UI: return GL_R8UI;
        case TextureFormat::RG8UI: return GL_RG8UI;
        case TextureFormat::RGB8UI: return GL_RGB8UI;
        case TextureFormat::RGBA8UI: return GL_RGBA8UI;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F: return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL;
    }
    
    CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

/**
 * Convert the texture format to its corresponding OpenGL (pixel) data type.
 *
 * @param format The texture format.
 *
 * @return OpenGL (pixel) data type for the specific texture format.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline GLenum TextureFormatToOpenGLDataType(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None: return 0;
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI: return GL_UNSIGNED_BYTE;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24STENCIL8: return GL_UNSIGNED_INT;
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
        
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::DEPTH32F: return GL_FLOAT;
    }
    
    CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

/**
 * Convert the texture format to its corresponding OpenGL depth (attachment) type.
 *
 * @param format The texture format.
 *
 * @return OpenGL depth (attachment) type.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline GLenum TextureFormatToOpenGLDepthType(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F: return GL_DEPTH_ATTACHMENT;
        case TextureFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
            
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
        case TextureFormat::RGBA8UI: break;
    }
    
    CORE_ASSERT(false, "Unknown depth texture format!");
    return false;
}

/**
 * Define the number of channels in the texture based on its format.
 *
 * @param format The texture format.
 *
 * @return Channels number.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline int TextureFormatToChannelNumber(TextureFormat format)
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
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return 0;
}

/**
 * Verify if a texture format can be represented as depth format.
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
    
    CORE_ASSERT(false, "Unknown texture format!");
    return false;
}

/**
 * Convert the texture wrap mode to its corresponding OpenGL type.
 *
 * @param mode The texture wrapping mode.
 *
 * @return OpenGL wrapping mode.
 *
 * @note If the input wrap mode is not recognized, the function will assert with an error.
 */
inline GLenum TextureWrapToOpenGLType(TextureWrap wrap)
{
    switch (wrap)
    {
        case TextureWrap::None: return 0;
        case TextureWrap::Repeat: return GL_REPEAT;
        case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
    }
    
    CORE_ASSERT(false, "Unknown texture wrap mode!");
    return 0;
}

/**
 * Convert the texture filter mode to its corresponding OpenGL type.
 *
 * @param mode The texture filtering mode.
 *
 * @return OpenGL filtering mode.
 *
 * @note If the input filter mode is not recognized, the function will assert with an error.
 */
inline GLenum TextureFilterToOpenGLType(TextureFilter filter, bool useMipmaps)
{
    // If mipmaps are generated, return its corresponding filtering
    // TODO: support more filters when using mipmaps
    if (useMipmaps)
    {
        switch (filter)
        {
            case TextureFilter::None: return 0;
            case TextureFilter::Nearest: return GL_NEAREST_MIPMAP_NEAREST;
            case TextureFilter::Linear: return GL_LINEAR_MIPMAP_LINEAR;
        }
    }
    // If no mipmaps are used, return a normal filtering
    else
    {
        switch (filter)
        {
            case TextureFilter::None: return 0;
            case TextureFilter::Nearest: return GL_NEAREST;
            case TextureFilter::Linear: return GL_LINEAR;
        }
    }
    
    CORE_ASSERT(false, "Unknown texture filter mode!");
    return 0;
}
} // namespace OpenGL

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
inline void* AllocateBufferForFormat(TextureFormat format, unsigned int bufferSize)
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
        case TextureFormat::RGBA8UI: return static_cast<void*>(new char[bufferSize]);
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24STENCIL8: return static_cast<void*>(new int[bufferSize]);
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
        
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::DEPTH32F: return static_cast<void*>(new float[bufferSize]);
    }
    
    CORE_ASSERT(false, "Unknown texture format!");
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
inline void DeallocateBufferForFormat(TextureFormat format, void* buffer)
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
            CORE_ASSERT(false, "Unknown texture format!");
            break;
    }
}

} // namespace utils
