#pragma once

#include "Foundation/Renderer/RendererAPI.h"

#include "Platform/Metal/MetalContext.h"

/**
 * Concrete implementation of the RendererAPI interface for Metal.
 *
 * The `MetalRendererAPI` class implements the `RendererAPI` abstract methods
 * specifically for the Metal rendering API.
 */
class MetalRendererAPI : public RendererAPI
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Create a Metal renderer API.
    MetalRendererAPI() = default;
    /// @brief Delete the Metal renderer API.
    virtual ~MetalRendererAPI() = default;
    
    // Initialization
    // ----------------------------------------
    void Init() override;
    
    // Render
    // ----------------------------------------
    void SetRenderTarget(const RenderTargetBuffers& targets = {}) override;
    void SetRenderTarget(const glm::vec4& color,
                         const RenderTargetBuffers& targets = {}) override;
    
    void SetRenderTarget(const RenderTargetBuffers& targets,
                         const std::shared_ptr<FrameBuffer>& framebuffer) override;
    void SetRenderTarget(const glm::vec4& color,
                         const RenderTargetBuffers& targets,
                         const std::shared_ptr<FrameBuffer>& framebuffer) override;
    
    void EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer) override;
    
    // Draw
    // ----------------------------------------
    void Draw(const std::shared_ptr<Drawable>& drawable,
              const PrimitiveType &primitive = PrimitiveType::Triangle) override;
    
    // Setter(s)
    // ----------------------------------------
    void SetViewport(unsigned int x, unsigned int y,
                     unsigned int width, unsigned int height) override;
    
    void SetDepthTesting(const bool enabled) override;
    
    // Renderer API variables
    // ----------------------------------------
private:
    ///< Metal context.
    MetalContext* m_Context;
};
