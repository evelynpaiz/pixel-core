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
    
    // Getter(s)
    // ----------------------------------------
    static bool IsRendering() { return s_IsRendering; }
    
    // Setter(s)
    // ----------------------------------------
    void SetClearColor(const glm::vec4& color) override;
    
    void SetViewport(const unsigned int x, const unsigned int y,
                     const unsigned int width, const unsigned int height) override;
    
    void SetDepthTesting(const bool enabled,
                         const DepthFunction function) override;
    
    // Render
    // ----------------------------------------
    void BeginRenderPass() override;
    void EndRenderPass() override;
    
    void Clear(const RenderTargetBuffers& targets) override;
    
    void Draw(const std::shared_ptr<Drawable>& drawable,
              const PrimitiveType &primitive = PrimitiveType::Triangle) override;
    
    /*
     void SetRenderTarget(const RenderTargetBuffers& targets) override;
     void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets) override;
     
     void SetRenderTarget(const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) override;
     void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) override;
     */
    
    // OpenGL API variables
    // ----------------------------------------
private:
    ///< Rendering flag.
    static bool s_IsRendering;
};

} // namespace pixc
