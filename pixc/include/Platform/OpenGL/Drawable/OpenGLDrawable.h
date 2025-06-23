#pragma once

#include "Foundation/Renderer/Drawable/Drawable.h"
#include "Platform/OpenGL/Buffer/OpenGLVertexArray.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a drawable object specifically for OpenGL rendering.
 *
 * This class derives from `Drawable` and implements the necessary functionality
 * to manage and bind vertex buffers, an index buffer, and shaders within
 * the OpenGL rendering context.
 *
 * Copying or moving `OpenGLDrawable` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class OpenGLDrawable : public Drawable
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Constructs a drawable object, initializing its vertex array.
    OpenGLDrawable() : Drawable()
    {
        m_VertexArray = std::make_shared<OpenGLVertexArray>();
    }
    /// @brief Destroys the drawable object.
    ~OpenGLDrawable() override = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Binds the drawable by binding its vertex array.
    void Bind() const override { m_VertexArray->Bind(); }
    /// @brief Un-binds the drawable by un-binding its vertex array.
    void Unbind() const override { m_VertexArray->Unbind(); }
    
private:
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Add a vertex buffer to the vertex array.
    /// @param vbo The vertex buffer.
    void SetVertexAttributes(const std::shared_ptr<VertexBuffer>& vbo) override
    {
        m_VertexArray->SetVertexAttributes(vbo, m_Index);
    };
    
    // OpenGL drawable variables
    // ----------------------------------------
private:
    ///< Vertex array.
    std::shared_ptr<OpenGLVertexArray> m_VertexArray;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLDrawable);
};

} // namespace pixc
