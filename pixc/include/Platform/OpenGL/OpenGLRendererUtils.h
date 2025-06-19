#pragma once

#include "Foundation/Renderer/RendererUtils.h"

#include <GL/glew.h>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Utility functions related to OpenGL rendering operations.
 */
namespace utils { namespace graphics { namespace gl
{
/**
 * Convert the data type to its corresponding OpenGL type.
 *
 * @param dataType The type of data.
 *
 * @return OpenGL data type.
 *
 * @note If the input DataType value is not recognized, the function will assert with an error.
 */
inline GLenum ToOpenGLType(DataType dataType)
{
    switch (dataType)
    {
        case DataType::None:  return 0;
        case DataType::Bool:  return GL_BOOL;
        case DataType::Int:   return GL_INT;
        case DataType::Float: return GL_FLOAT;
        case DataType::Vec2:  return GL_FLOAT;
        case DataType::Vec3:  return GL_FLOAT;
        case DataType::Vec4:  return GL_FLOAT;
        case DataType::Mat2:  return GL_FLOAT;
        case DataType::Mat3:  return GL_FLOAT;
        case DataType::Mat4:  return GL_FLOAT;
            
    }
    
    CORE_ASSERT(false, "Unknown data type!");
    return 0;
}

/**
 * Convert an OpenGL type to its corresponding data type.
 *
 * @param glType The OpenGL data type.
 *
 * @return The corresponding DataType.
 *
 * @note If the input glType value is not recognized, the function will assert with an error.
 */
inline DataType ToDataType(GLenum glType)
{
    switch (glType)
    {
        case GL_BOOL:       return DataType::Bool;
        case GL_INT:        return DataType::Int;
        case GL_FLOAT:      return DataType::Float;
        case GL_FLOAT_VEC2: return DataType::Vec2;
        case GL_FLOAT_VEC3: return DataType::Vec3;
        case GL_FLOAT_VEC4: return DataType::Vec4;
        case GL_FLOAT_MAT2: return DataType::Mat2;
        case GL_FLOAT_MAT3: return DataType::Mat3;
        case GL_FLOAT_MAT4: return DataType::Mat4;
            
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_2D_SHADOW:
            return DataType::Int;
    }
    
    CORE_ASSERT(false, "Unknown OpenGL data type!");
    return DataType::None;
}

/**
 * Convert the buffers state to its corresponding OpenGL clear mask.
 *
 * @param targets Active rendering targets.
 *
 * @return Bitwise OR of masks that indicate the buffers to be cleared.
 */
inline GLbitfield ToOpenGLClearMask(const RenderTargetBuffers& targets) {
    GLbitfield mask = 0;
    if (targets.colorBufferActive) {
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (targets.depthBufferActive) {
        mask |= GL_DEPTH_BUFFER_BIT;
    }
    if (targets.stencilBufferActive) {
        mask |= GL_STENCIL_BUFFER_BIT;
    }
    return mask;
}

/**
 * Convert a primitive to the corresponding OpenGL primitive type.
 *
 * @param primitiveType The primitive value to be converted.
 * @return The corresponding OpenGL primitive type as a GLenum.
 */
inline GLenum ToOpenGLPrimitive(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case PrimitiveType::Point:         return GL_POINTS;
        case PrimitiveType::Line:          return GL_LINES;
        case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
        case PrimitiveType::Triangle:      return GL_TRIANGLES;
        case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
    }
    
    CORE_ASSERT(false, "Unknown primitive type!");
    return 0;
}

/**
 * Convert the face culling mode to its corresponding OpenGL mode.
 *
 * @param culling The face culling mode to be converted.
 *
 * @return The corresponding OpenGL face culling mode.
 *
 * @note If the input face culling mode is not recognized, the function will assert with an error.
 */
inline GLenum ToOpenGLCulling(FaceCulling culling)
{
    switch (culling)
    {
        case FaceCulling::Front:        return GL_FRONT;
        case FaceCulling::Back:         return GL_BACK;
        case FaceCulling::FrontAndBack: return GL_FRONT_AND_BACK;
    }
    
    CORE_ASSERT(false, "Unknown face culling mode!");
    return 0;
}

/**
 * Convert the depth function to its corresponding OpenGL function.
 *
 * @param depth The depth function mode to be converted.
 *
 * @return The corresponding OpenGL depth function.
 *
 * @note If the input depth function is not recognized, the function will assert with an error.
 */
inline GLenum ToOpenGLDepthFunc(DepthFunction depth)
{
    switch (depth)
    {
        case DepthFunction::Always:   return GL_ALWAYS;
        case DepthFunction::Never:    return GL_NEVER;
        case DepthFunction::Less:     return GL_LESS;
        case DepthFunction::Equal:    return GL_EQUAL;
        case DepthFunction::LEqual:   return GL_LEQUAL;
        case DepthFunction::Greater:  return GL_GREATER;
        case DepthFunction::NotEqual: return GL_NOTEQUAL;
        case DepthFunction::GEqual:   return GL_GEQUAL;
    }
    
    CORE_ASSERT(false, "Unknown depth function!");
    return 0;
}

} // namespace gl
} // namespace graphics
} // namespace utils
} // namespace pixc
