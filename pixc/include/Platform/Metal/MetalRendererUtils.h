#pragma once

#include "Common/Renderer/RendererUtils.h"

#include <Metal/Metal.h>

namespace utils { namespace graphics { namespace mtl
{
/**
 * Convert the data type to its corresponding Metal format.
 *
 * @param dataType The type of data.
 *
 * @return Metal data format.
 *
 * @note If the input DataType value is not recognized, the function will assert with an error.
 */
inline MTLVertexFormat ToMetalFormat(DataType dataType)
{
    switch (dataType)
    {
        case DataType::None:  return MTLVertexFormatInvalid;
            
        case DataType::Bool:  return MTLVertexFormatInt;
        case DataType::Int:   return MTLVertexFormatInt;
        case DataType::Float: return MTLVertexFormatFloat;
        case DataType::Vec2:  return MTLVertexFormatFloat2;
        case DataType::Vec3:  return MTLVertexFormatFloat3;
        case DataType::Vec4:  return MTLVertexFormatFloat4;
            
        // Not supported by Metal
        case DataType::Mat2:  return MTLVertexFormatInvalid;
        case DataType::Mat3:  return MTLVertexFormatInvalid;
        case DataType::Mat4:  return MTLVertexFormatInvalid;
    }
    
    CORE_ASSERT(false, "Unknown data type!");
    return MTLVertexFormatInvalid;
}

/**
 * Convert a Metal data type to its corresponding data type.
 *
 * @param mtlTyme The Metal data type.
 *
 * @return The corresponding DataType.
 *
 * @note If the input mtlType value is not recognized, the function will assert with an error.
 */
inline DataType ToDataType(MTLDataType mtlType)
{
    switch (mtlType)
    {
        case MTLDataTypeInt:        return DataType::Int;
        case MTLDataTypeFloat:      return DataType::Float;
        case MTLDataTypeFloat2:     return DataType::Vec2;
        case MTLDataTypeFloat3:     return DataType::Vec3;
        case MTLDataTypeFloat4:     return DataType::Vec4;
        case MTLDataTypeFloat2x2:   return DataType::Mat2;
        case MTLDataTypeFloat3x3:   return DataType::Mat3;
        case MTLDataTypeFloat4x4:   return DataType::Mat4;
    }
    
    CORE_ASSERT(false, "Unknown (or unsupported) Metal data type!");
    return DataType::None;
}

/**
 * Convert a primitive to the corresponding Metal primitive type.
 *
 * @param primitiveType The primitive value to be converted.
 * @return The corresponding Metal primitive type as a GLenum.
 */
inline MTLPrimitiveType ToMetalPrimitive(PrimitiveType primitiveType) 
{
    switch (primitiveType)
    {
        case PrimitiveType::Point:         return MTLPrimitiveTypePoint;
        case PrimitiveType::Line:          return MTLPrimitiveTypeLine;
        case PrimitiveType::LineStrip:     return MTLPrimitiveTypeLineStrip;
        case PrimitiveType::Triangle:      return MTLPrimitiveTypeTriangle;
        case PrimitiveType::TriangleStrip: return MTLPrimitiveTypeTriangleStrip;
    }

    CORE_ASSERT(false, "Unknown primitive type!");
    return MTLPrimitiveTypeTriangle;
}

} // namespace mtl
} // namespace graphics
} // namespace utils
