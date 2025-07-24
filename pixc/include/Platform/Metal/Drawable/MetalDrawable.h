#pragma once

#include "Foundation/Renderer/Drawable/Drawable.h"

#include "Platform/Metal/MetalContext.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a drawable object designed for rendering using Apple's Metal framework.
 *
 * This class inherits from `Drawable` and provides a higher-level interface
 * for drawing geometry using Metal. It manages the creation, configuration,
 * and binding of Metal-specific rendering resources, such as pipeline states,
 * vertex buffers, and shaders.
 *
 * Copying or moving `MetalDrawable` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class MetalDrawable : public Drawable
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalDrawable();
    virtual ~MetalDrawable() override;
    
    // Usage
    // ----------------------------------------
    void Bind() const override;
    void Unbind() const override {};
    
    // Setter(s)
    // ----------------------------------------
    void SetShader(const std::shared_ptr<Shader>& shader) override;
    
    // Getter(s)
    // ----------------------------------------
    void* GetPipelineState() const;
    
    // Argument(s) and Uniform(s)
    // ----------------------------------------
    void InitUniformBuffers();
    void BindUniformBuffers() const;
    void UpdateUniformBuffers() const;
    
private:
    // Setter(s)
    // ----------------------------------------
    void SetVertexAttributes(const std::shared_ptr<VertexBuffer>& vbo) override;
    void SetPipelineState() const;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Gets the index of a vertex buffer in the internal list.
    /// @param  vbo  A shared pointer to the `VertexBuffer` to search for.
    /// @return The index of the vertex buffer if found, -1 otherwise.
    int GetVertexBufferIndex(const std::shared_ptr<VertexBuffer>& vbo) const
    {
        // Search for the vertex buffer
        auto it = std::find(m_VertexBuffers.begin(), m_VertexBuffers.end(), vbo);
        
        // Check if the vertex buffer was found
        if (it != m_VertexBuffers.end()) {
            // Calculate and return the index
            return static_cast<int>(std::distance(m_VertexBuffers.begin(), it));
        }
        // Vertex buffer not found, returns -1
        return -1;
    }
    
    // Metal drawable variables
    // ----------------------------------------
private:
    ///< Holds the core Metal drawable object to render a geometry.
    struct DrawableState;
    std::shared_ptr<DrawableState> m_State;
    
    ///< Metal context.
    MetalContext* m_Context;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalDrawable);
};

} // namespace pixc
