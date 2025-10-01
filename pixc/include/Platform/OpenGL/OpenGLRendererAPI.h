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
    
    void SetViewport(const uint32_t x, const uint32_t y,
                     const uint32_t width, const uint32_t height) override;
    
    void EnableDepthTesting(const bool enabled) override;
    void SetDepthFunction(const DepthFunction function) override;
    
    void SetFaceCulling(const FaceCulling mode) override;
    void SetCubeMapSeamless(const bool enabled) override;
    
    // Render
    // ----------------------------------------
    void BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer) override;
    
    void Clear(const RenderTargetMask targets) override;
    
    void Draw(const std::shared_ptr<Drawable>& drawable,
              const PrimitiveType &primitive = PrimitiveType::Triangle) override;
    
    // OpenGL API variables
    // ----------------------------------------
private:
    ///< Rendering flag.
    static bool s_IsRendering;
};

} // namespace pixc
