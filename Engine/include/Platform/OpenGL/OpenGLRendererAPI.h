#pragma once

#include "Foundation/Renderer/RendererAPI.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Concrete implementation of the RendererAPI interface for OpenGL.
 *
 * The `OpenGLRendererAPI` class implements the `RendererAPI` abstract methods
 * specifically for the OpenGL rendering API.
 */
class OpenGLRendererAPI : public RendererAPI
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Create an OpenGL renderer API.
    OpenGLRendererAPI() = default;
    /// @brief Delete the OpenGL renderer API.
    virtual ~OpenGLRendererAPI() = default;
    
    // Initialization
    // ----------------------------------------
    void Init() override;
    
    // Render
    // ----------------------------------------
    /*
     void SetRenderTarget(const RenderTargetBuffers& targets) override;
     void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets) override;
     
     void SetRenderTarget(const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) override;
     void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) override;
     
     // Draw
     // ----------------------------------------
     void Draw(const std::shared_ptr<Drawable>& drawable,
     const PrimitiveType &primitive = PrimitiveType::Triangle) override;
     
     // Setter(s)
     // ----------------------------------------
     void SetViewport(unsigned int x, unsigned int y,
     unsigned int width, unsigned int height) override;
     
     void SetDepthTesting(const bool enabled) override;
     */
    
private:
    // Clear
    // ----------------------------------------
    //void Clear(const RenderTargetBuffers& targets);
    //void Clear(const glm::vec4& color,
    //           const RenderTargetBuffers& targets);
};

} // namespace pixc
