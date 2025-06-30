#pragma once

#import <Metal/Metal.h>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @namespace utils::textures::gl
 * @brief Utility functions related to texture operations in Metal.
 */
namespace utils { namespace textures { namespace mtl {

/**
 * @brief Converts a TextureType enumeration value to its corresponding Metal texture type.
 *
 * @param type The TextureType to convert.
 * @return The equivalent Metal texture type.
 *
 * @note Asserts if the input `type` is not a valid TextureType.
 */
inline MTLTextureType ToMetalTextureType(TextureType type)
{
    switch (type) {
        case TextureType::None:                     return (MTLTextureType)0;
        case TextureType::TEXTURE1D:                return MTLTextureType1D;
        case TextureType::TEXTURE2D:                return MTLTextureType2D;
        case TextureType::TEXTURE2D_MULTISAMPLE:    return MTLTextureType2DMultisample;
        case TextureType::TEXTURE3D:                return MTLTextureType3D;
        case TextureType::TEXTURECUBE:              return MTLTextureTypeCube;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture type!");
    return (MTLTextureType)0;
}

/**
 * @brief Convert the texture format to its corresponding Metal pixel format type.
 *
 * @param format The texture format.
 *
 * @return Metal pixel format type.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline MTLPixelFormat ToMetalPixelFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::None:             return MTLPixelFormatInvalid;
            
        case TextureFormat::R8:               return MTLPixelFormatR8Unorm;
        case TextureFormat::RG8:              return MTLPixelFormatRG8Unorm;
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:            return MTLPixelFormatRGBA8Unorm;
            
        case TextureFormat::R16F:             return MTLPixelFormatR16Float;
        case TextureFormat::RG16F:            return MTLPixelFormatRG16Float;
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:          return MTLPixelFormatRGBA16Float;
            
        case TextureFormat::R32F:             return MTLPixelFormatR32Float;
        case TextureFormat::RG32F:            return MTLPixelFormatRG32Float;
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:          return MTLPixelFormatRGBA32Float;
            
        case TextureFormat::R8UI:             return MTLPixelFormatR8Uint;
        case TextureFormat::RG8UI:            return MTLPixelFormatRG8Uint;
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:          return MTLPixelFormatRGBA8Uint;
            
        case TextureFormat::DEPTH16:          return MTLPixelFormatDepth16Unorm;
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:         return MTLPixelFormatDepth32Float;
        case TextureFormat::DEPTH24STENCIL8:  return MTLPixelFormatDepth24Unorm_Stencil8;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return MTLPixelFormatInvalid;
}

/**
 * @brief Convert the texture wrap mode to its corresponding Metal type.
 *
 * @param mode The texture wrapping mode.
 *
 * @return Metal wrapping mode.
 *
 * @note If the input wrap mode is not recognized, the function will assert with an error.
 */
inline MTLSamplerAddressMode ToMetalWrap(TextureWrap wrap)
{
    switch (wrap)
    {
        case TextureWrap::None:             return (MTLSamplerAddressMode)0;
        case TextureWrap::Repeat:           return MTLSamplerAddressModeRepeat;
        case TextureWrap::MirroredRepeat:   return MTLSamplerAddressModeMirrorRepeat;
        case TextureWrap::ClampToEdge:      return MTLSamplerAddressModeClampToEdge;
        case TextureWrap::ClampToBorder:    return MTLSamplerAddressModeClampToBorderColor;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture wrap mode!");
    return (MTLSamplerAddressMode)0;
}

/**
 * @brief Convert the texture filter mode to its corresponding Metal type.
 *
 * @param mode The texture filtering mode.
 *
 * @return Metal filtering mode.
 *
 * @note If the input filter mode is not recognized, the function will assert with an error.
 */
inline MTLSamplerMinMagFilter ToMetalFilter(TextureFilter filter)
{
    switch (filter)
    {
        case TextureFilter::None:       return (MTLSamplerMinMagFilter)0;
        case TextureFilter::Nearest:    return MTLSamplerMinMagFilterNearest;
        case TextureFilter::Linear:     return MTLSamplerMinMagFilterLinear;
            
            // TODO: support more options for texture filtering.
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture filter mode!");
    return (MTLSamplerMinMagFilter)0;
}

/**
 *  @brief Creates an MTLRegion based on a TextureSpecification.
 *
 *  @param spec The TextureSpecification defining the texture's type and dimensions.
 *
 *  @return An MTLRegion representing the specified texture region.
 *
 * @note If the input type is not recognized, the function will assert with an error.
 */
inline MTLRegion GetMetalRegion(const TextureSpecification& spec)
{
    switch (spec.Type) {
        case TextureType::None:
            return MTLRegionMake2D(0, 0, 0, 0);
        case TextureType::TEXTURE1D:
            return MTLRegionMake1D(0, spec.Width);
        case TextureType::TEXTURE2D:
        case TextureType::TEXTURE2D_MULTISAMPLE:
            return MTLRegionMake2D(0, 0, spec.Width, spec.Height);
        case TextureType::TEXTURE3D:
            return MTLRegionMake3D(0, 0, 0, spec.Width, spec.Height, spec.Depth);
        case TextureType::TEXTURECUBE:
            return MTLRegionMake2D(0, 0, spec.Width, spec.Height);
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture type!");
    return MTLRegionMake2D(0, 0, 0, 0);
}

/**
 * @brief Define the number of channels in the texture based on its format.
 *
 * @param format The texture format.
 *
 * @return Channels number.
 *
 * @note If the input format is not recognized, the function will assert with an error.
 */
inline unsigned int GetMetalChannelCount(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::R32F:
        case TextureFormat::R16F:
        case TextureFormat::R8:
        case TextureFormat::R8UI:               return 1;
            
        case TextureFormat::DEPTH16:
        case TextureFormat::DEPTH24:
        case TextureFormat::DEPTH32:
        case TextureFormat::DEPTH32F:
        case TextureFormat::DEPTH24STENCIL8:    return 1;
            
        case TextureFormat::RG32F:
        case TextureFormat::RG16F:
        case TextureFormat::RG8:
        case TextureFormat::RG8UI:              return 2;
            
        case TextureFormat::RGB32F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB8:
        case TextureFormat::RGB8UI:
            
        case TextureFormat::RGBA32F:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA8UI:            return 4;
            
        case TextureFormat::None:               return 0;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown (or unsupported) texture format!");
    return 0;
}

} // namespace mtl
} // namespace texturing
} // namespace utils
} // namespace pixc
