#pragma once

#include "Foundation/Renderer/Buffer/VertexBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of `VertexBuffer` for the OpenGL rendering API.
 *
 * This class handles the creation, binding, and management of vertex buffers specifically
 * for OpenGL. It uses OpenGL functions to interact with the GPU.
 *
 * Copying or moving `OpenGLVertexBuffer` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class OpenGLVertexBuffer : public VertexBuffer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLVertexBuffer(const void *vertices, const uint32_t size,
                       const uint32_t count);
    virtual ~OpenGLVertexBuffer();
    
    // Usage
    // ----------------------------------------
    void Bind() const override;
    void Unbind() const override;
    
    // Vertex buffer variables
    // ----------------------------------------
private:
    ///< ID of the vertex buffer.
    uint32_t m_ID = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLVertexBuffer);
};

} // namespace pixc
