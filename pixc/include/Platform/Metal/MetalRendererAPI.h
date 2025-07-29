#pragma once

#include "Foundation/Renderer/RendererAPI.h"

#include "Platform/Metal/MetalContext.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

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
    
    // Setter(s)
    // ----------------------------------------
    void SetClearColor(const glm::vec4& color) override;
    
    void SetViewport(const unsigned int x, const unsigned int y,
                     const unsigned int width, const unsigned int height) override;
    
    void SetDepthTesting(const bool enabled,
                         const DepthFunction function) override;
    
    // Render
    // ----------------------------------------
    void BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer) override;
    void EndRenderPass() override;
    
    void Clear(const RenderTargetBuffers& targets) override;
    
    void Draw(const std::shared_ptr<Drawable>& drawable,
              const PrimitiveType &primitive = PrimitiveType::Triangle) override;
    
private:
    // Initialization
    // ----------------------------------------
    void CreateDepthTexture();
    void* GetOrCreateDepthState();
    
    // Renderer API variables
    // ----------------------------------------
private:
    ///< Metal context.
    MetalContext* m_Context;
    
    ///< Holds the core Metal objects required for rendering.
    struct MetalRendererState;
    std::shared_ptr<MetalRendererState> m_State;
};

} // namespace pixc
