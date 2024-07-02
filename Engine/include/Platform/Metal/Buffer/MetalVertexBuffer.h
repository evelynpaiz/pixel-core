#pragma once

#include "Common/Renderer/Buffer/VertexBuffer.h"

/**
 * Concrete implementation of `VertexBuffer` for the OpenGL rendering API.
 *
 * This class handles the creation, binding, and management of vertex buffers specifically
 * for OpenGL. It uses OpenGL functions to interact with the GPU.
 *
 * Copying or moving `OpenGLVertexBuffer` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class MetalVertexBuffer : public VertexBuffer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalVertexBuffer(const void *vertices, const uint32_t size,
                       const uint32_t count);
    ///@brief Delete the vertex buffer.
    ~MetalVertexBuffer() override = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Bind the index buffer.
    /// @note Not necessary for Metal API.
    void Bind() const override {};
    /// @brief Unbind the index buffer.
    /// @note Not necessary for Metal API.
    void Unbind() const override {};
    
    // Getter(s)
    // ----------------------------------------
    void* GetBuffer() const { return m_Buffer; }
    
    // Vertex buffer variables
    // ----------------------------------------
private:
    ///< Index buffer.
    void* m_Buffer;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    MetalVertexBuffer(const MetalVertexBuffer&) = delete;
    MetalVertexBuffer(MetalVertexBuffer&&) = delete;

    MetalVertexBuffer& operator=(const MetalVertexBuffer&) = delete;
    MetalVertexBuffer& operator=(MetalVertexBuffer&&) = delete;
};
