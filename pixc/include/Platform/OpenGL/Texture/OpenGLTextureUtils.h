#pragma once

#include <GL/glew.h>

/**
 * Utility functions related to texture operations.
 */
namespace utils { namespace textures { namespace gl {
/**
 * Converts a TextureType enumeration value to its corresponding OpenGL texture target.
 *
 * @param type The TextureType to convert.
 * @return The equivalent OpenGL texture target.
 *
 * @note Asserts if the input `type` is not a valid TextureType.
 */
inline GLenum ToOpenGLTextureTarget(TextureType type)
{
    switch (type) {
        case TextureType::None:                     return 0;
        case TextureType::TEXTURE1D:                return (GLenum)GL_TEXTURE_1D;
        case TextureType::TEXTURE2D:                return (GLenum)GL_TEXTURE_2D;
        case TextureType::TEXTURE2D_MULTISAMPLE:    return (GLenum)GL_TEXTURE_2D_MULTISAMPLE;
        case TextureType::TEXTURE3D:                return (GLenum)GL_TEXTURE_3D;
        case TextureType::TEXTURECUBE:              return (GLenum)GL_TEXTURE_CUBE_MAP;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture type!");
    return 0;
}

/**
 * Convert the texture format to its corresponding OpenGL (base) format type.
 *
 * @param format The texture format.
 *
 * @return OpenGL texture (base) format type.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline GLenum ToOpenGLBaseFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None:               return 0;
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:               return GL_RED;
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:              return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:             return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:            return GL_RGBA;
            
        case TextureFormat::R8UI:               return GL_RED_INTEGER;
        case TextureFormat::RG8UI:              return GL_RG_INTEGER;
        case TextureFormat::RGB8UI:             return GL_RGB_INTEGER;
        case TextureFormat::RGBA8UI:            return GL_RGBA_INTEGER;
            
        case TextureFormat::DEPTH16:            return GL_DEPTH_COMPONENT16;
        case TextureFormat::DEPTH24:            return GL_DEPTH_COMPONENT24;
        case TextureFormat::DEPTH32:            return GL_DEPTH_COMPONENT32;
        case TextureFormat::DEPTH32F:           return GL_DEPTH_COMPONENT32F;
        case TextureFormat::DEPTH24STENCIL8:    return GL_DEPTH24_STENCIL8;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
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
inline GLenum ToOpenGLInternalFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None:               return 0;
        case TextureFormat::R8:                 return GL_R8;
        case TextureFormat::RG8:                return GL_RG8;
        case TextureFormat::RGB8:               return GL_RGB8;
        case TextureFormat::RGBA8:              return GL_RGBA16;
            
        case TextureFormat::R16F:               return GL_R16F;
        case TextureFormat::RG16F:              return GL_RG16F;
        case TextureFormat::RGB16F:             return GL_RGB16F;
        case TextureFormat::RGBA16F:            return GL_RGBA16F;
        
        case TextureFormat::R32F:               return GL_R32F;
        case TextureFormat::RG32F:              return GL_RG32F;
        case TextureFormat::RGB32F:             return GL_RGB32F;
        case TextureFormat::RGBA32F:            return GL_RGBA32F;
            
        case TextureFormat::R8UI:               return GL_R8UI;
        case TextureFormat::RG8UI:              return GL_RG8UI;
        case TextureFormat::RGB8UI:             return GL_RGB8UI;
        case TextureFormat::RGBA8UI:            return GL_RGBA8UI;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:           return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH24STENCIL8:    return GL_DEPTH_STENCIL;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
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
inline GLenum ToOpenGLDataFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None:               return 0;
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:            return GL_UNSIGNED_BYTE;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH24STENCIL8:    return GL_UNSIGNED_INT;
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        case TextureFormat::DEPTH32F:           return GL_FLOAT;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
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
inline GLenum ToOpenGLDepthAttachment(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:           return GL_DEPTH_ATTACHMENT;
        case TextureFormat::DEPTH24STENCIL8:    return GL_DEPTH_STENCIL_ATTACHMENT;
            
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
        case TextureFormat::RGBA8UI:            break;
    }
    
    CORE_ASSERT(false, "Format is not defined as a depth format!");
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
inline GLenum ToOpenGLWrap(TextureWrap wrap)
{
    switch (wrap)
    {
        case TextureWrap::None:             return 0;
        case TextureWrap::Repeat:           return GL_REPEAT;
        case TextureWrap::MirroredRepeat:   return GL_MIRRORED_REPEAT;
        case TextureWrap::ClampToEdge:      return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampToBorder:    return GL_CLAMP_TO_BORDER;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture wrap mode!");
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
inline GLenum ToOpenGLFilter(TextureFilter filter, bool useMipmaps)
{
    switch (filter)
    {
        case TextureFilter::None:       
            return 0;
        case TextureFilter::Nearest:
            return useMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        case TextureFilter::Linear:
            return useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
            
        // TODO: support more options for texture filtering.
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) texture filter mode!");
    return 0;
}

} // namespace gl
} // namespace texturing
} // namespace utils
