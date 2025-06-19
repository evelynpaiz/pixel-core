#pragma once

#include "Common/Renderer/Buffer/VertexBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Represents a vertex array that describes how vertex attributes are stored in vertex buffer(s).
 *
 * The `OpenGLVertexArray` class manages the overall state and bindings related to vertex
 * attributes. It allows for adding multiple vertex buffers and setting an index buffer. Use this class
 * to define the complete layout of vertex data for rendering a `Mesh` using the OpenGL API.
 *
 * Copying or moving `OpenGLVertexArray` objects is disabled to ensure single ownership and
 * prevent unintended duplication.
 */
class OpenGLVertexArray
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLVertexArray();
    ~OpenGLVertexArray();
    
    // Usage
    // ----------------------------------------
    void Bind() const;
    void Unbind() const;
    
    // Setter(s)
    // ----------------------------------------
    void SetVertexAttributes(const std::shared_ptr<VertexBuffer>& vbo,
                             unsigned int& index);
    
    // Vertex array variables
    // ----------------------------------------
private:
    ///< ID of the vertex array.
    unsigned int m_ID = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLVertexArray);
};

} // namespace pixc
