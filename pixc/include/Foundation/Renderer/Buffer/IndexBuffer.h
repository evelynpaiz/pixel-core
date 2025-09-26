#pragma once

#include "Foundation/Core/ClassUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents an index buffer for efficient rendering.
 *
 * The `IndexBuffer` class provides an abstract interface for managing index buffers,
 * which store indices that define the order in which vertices are rendered. This
 * allows for efficient reuse of vertex data, reducing memory usage and improving
 * rendering performance.
 *
 * This class defines the common interface for index buffers across different rendering APIs.
 * Concrete implementations for specific APIs (e.g., OpenGL, Vulkan) inherit from this
 * class and handle the actual buffer creation and management logic.
 *
 * Copying or moving `IndexBuffer` objects is disabled to ensure single ownership and prevent
 * unintended buffer duplication.
 */
class IndexBuffer
{
public:
    // Constructor(s)
    // ----------------------------------------
    static std::shared_ptr<IndexBuffer> Create(const uint32_t *indices,
                                               const uint32_t count);
    // Destructor
    // ----------------------------------------
    /// @brief Delete the vertex buffer.
    virtual ~IndexBuffer() = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Pure virtual function for binding the index buffer.
    virtual void Bind() const = 0;
    /// @brief Pure virtual function for unbinding the index buffer.
    virtual void Unbind() const = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the number of indices.
    /// @return The count of indices.
    uint32_t GetCount() const { return m_Count; }
    
protected:
    // Base constructor
    // ----------------------------------------
    /// @brief Generate an index buffer.
    /// @param count Number of indices.
    IndexBuffer(const uint32_t count) : m_Count(count) {};
    
    // Index buffer variables
    // ----------------------------------------
protected:
    ///< Number of indices (element count).
    uint32_t m_Count = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(IndexBuffer);
};

} // namespace pixc
