#pragma once

#include "Foundation/Core/ClassUtils.h"

#include "Foundation/Renderer/Buffer/Buffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a vertex buffer for storing vertex data.
 *
 * The `VertexBuffer` class provides an abstract interface for managing vertex buffers, which
 * are used to store vertex attribute data (position, color, texture coordinates, etc.) on the GPU.
 * This data is used by the graphics pipeline during rendering.
 *
 * This class defines the common interface for vertex buffers across different rendering APIs.
 * Concrete implementations for specific APIs (e.g., OpenGL, Vulkan) inherit from this class
 * and provide the actual buffer creation and management logic.
 *
 * Copying or moving `VertexBuffer` objects is disabled to ensure single ownership and prevent
 * unintended buffer duplication.
 */
class VertexBuffer
{
public:
    // Constructor(s)
    // ----------------------------------------
    static std::shared_ptr<VertexBuffer> Create(const void *vertices, const uint32_t size,
                                                const uint32_t count);
    // Destructor
    // ----------------------------------------
    /// @brief Delete the vertex buffer.
    virtual ~VertexBuffer() = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Pure virtual function for binding the vertex buffer.
    virtual void Bind() const = 0;
    /// @brief Pure virtual function for unbinding the vertex buffer.
    virtual void Unbind() const = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the number of vertices.
    /// @return The amount of vertices defined.
    uint32_t GetCount() const { return m_Count; }
    /// @brief Retrieve the current layout of the buffer, specifying the arrangement and format
    /// of vertex attributes within the buffer.
    /// @return The layout of the buffer.
    const BufferLayout& GetLayout() const { return m_Layout; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the layout of the buffer, specifying the arrangement and format
    /// of vertex attributes within the buffer.
    /// @param layout The buffer layout.
    void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
    
protected:
    // Base constructor
    // ----------------------------------------
    /// @brief Generate a vertex buffer.
    VertexBuffer(const uint32_t count) : m_Count(count) {};
    
    // Vertex buffer variables
    // ----------------------------------------
protected:
    ///< Number of vertices (element count).
    uint32_t m_Count = 0;
    ///< Layout for the vertex attributes.
    BufferLayout m_Layout;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(VertexBuffer);
};

} // namespace pixc
