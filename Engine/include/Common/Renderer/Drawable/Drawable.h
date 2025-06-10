#pragma once

#include "Common/Renderer/Buffer/VertexBuffer.h"
#include "Common/Renderer/Buffer/IndexBuffer.h"

#include "Common/Renderer/Shader/Shader.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a drawable object in a rendering system.
 *
 * The `Drawable` class serves as an abstract base class for objects that can be
 * drawn using a graphics API. It provides functionality for managing vertex buffers,
 * an index buffer, and a shader, as well as methods for binding and unbinding
 * the drawable for rendering.
 *
 * Derived classes must implement the `Bind`, `Unbind`, and `SetVertexAttributes`
 * methods to handle the specific binding and attribute setup for the target graphics API.
 *
 * Copying or moving `Drawable` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class Drawable
{
public:
    // Constructor(s)
    // ----------------------------------------
    static std::shared_ptr<Drawable> Create();
    
    // Destructor
    // ----------------------------------------
    /// @brief Delete the drawable object.
    virtual ~Drawable() = default;
    
    // Usage
    // ----------------------------------------
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get all the associated vertex buffers to this vertex array.
    /// @return Set of vertex buffers.
    const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const
    {
        return m_VertexBuffers;
    }
    /// @brief Get the index buffer associated to this vertex array.
    /// @return The index buffer.
    const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const
    {
        return m_IndexBuffer;
    }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Define a vertex buffer for the drawable object.
    /// @param vertices The vertex data.
    /// @param layout The layout of the vertex data in the buffer.
    template<typename VertexData>
    void AddVertexData(const std::vector<VertexData> &vertices,
                       const BufferLayout &layout)
    {
        // Verify that the size of the data is not higher than supported
        CORE_ASSERT((std::numeric_limits<unsigned int>::max() / sizeof(VertexData)) >= vertices.size(),
                    "Potential integer overflow in vertex buffer size calculation!");
        unsigned int size = static_cast<unsigned int>(vertices.size());
        
        // Create a vertex buffer containing the vertex data
        auto vertexBuffer = VertexBuffer::Create(vertices.data(), size * sizeof(VertexData), size);
        // Define the layout of the data
        vertexBuffer->SetLayout(layout);
        // Save a copy of the vertex buffer
        m_VertexBuffers.push_back(vertexBuffer);
        // Define the vertex attributes
        SetVertexAttributes(vertexBuffer);
    }
    /// @brief Define the index buffer for the drawable object.
    /// @param indices Set of indices.
    void SetIndexData(const std::vector<unsigned int> &indices)
    {
        // Verify that the size of the data is not higher than supported
        CORE_ASSERT((std::numeric_limits<unsigned int>::max() / sizeof(unsigned int)) >= indices.size(),
                    "Potential integer overflow in index buffer size calculation!");
        unsigned int size = static_cast<unsigned int>(indices.size());
        
        // Copy the index data in the buffer
        m_IndexBuffer = IndexBuffer::Create(indices.data(), size);
    }
    /// @brief Sets the shader used for shading the drawable object.
    /// @param shader The shader program.
    virtual void SetShader(const std::shared_ptr<Shader>& shader) { m_Shader = shader; }
    
protected:
    // Base constructor
    // ----------------------------------------
    /// @brief Create a drawable object.
    Drawable() = default;
    
    // Setter(s)
    // ----------------------------------------
    virtual void SetVertexAttributes(const std::shared_ptr<VertexBuffer>& vbo) = 0;
    
    // Vertex array variables
    // ----------------------------------------
protected:
    ///< Vertex attribute index.
    unsigned int m_Index = 0;
    
    ///< Linked vertex buffers (possible to have more than one).
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    ///< Linked index buffer.
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    
    ///< Linked shader program.
    std::shared_ptr<Shader> m_Shader;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Drawable);
};

} // namespace pixc
