#pragma once

#include "Foundation/Renderer/Camera/Camera.h"
#include "Foundation/Renderer/Light/Light.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

#include "Foundation/Scene/Viewport.h"
#include "Foundation/Scene/RenderPass.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class Scene
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Create a new scene.
    Scene(uint32_t width, uint32_t height,
          const std::filesystem::path& viewportShader = "");
    /// @brief Delete the specified scene.
    ~Scene() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the size (width) of the viewport.
    /// @return The width size.
    int GetViewportWidth() const { return m_Viewport->GetWidth(); };
    /// @brief Get the size (height) of the viewport.
    /// @return The height size.
    int GetViewportHeight() const { return m_Viewport->GetHeight(); };
    
    /// @brief Get the viewport where the scene is being rendered.
    /// @return The viewport.
    const std::shared_ptr<Viewport>& GetViewport() const { return m_Viewport; }
    
    /// @brief Get the camera used currently to render the scene.
    /// @return The active camera.
    const std::shared_ptr<Camera>& GetCamera() const { return m_Camera; }
    
    /// @brief Get the light sources defined in the scene.
    /// @return The scene's lights.
    LightLibrary& GetLights() { return m_Lights; }
    /// @brief Get the models defined in the scene.
    /// @return The scene's models.
    ModelLibrary& GetModels() { return m_Models; }
    
    /// @brief Get the framebuffers library.
    /// @return The defined framebuffers.
    FrameBufferLibrary& GetFrameBuffers() { return m_FrameBuffers; }
    
    /// @brief Get the render passes on the scene.
    /// @return The defined render passes with its specifications.
    RenderPassLibrary& GetRenderPasses() { return m_RenderPasses; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the camera used currently to render the scene.
    /// @param camera The active rendering camera.
    void SetCamera(const std::shared_ptr<Camera>& camera)
    {
        m_Camera = camera;
    }
    
    // Render
    // ----------------------------------------
    void Draw();
    
private:
    void Draw(const RenderPassSpecification& pass);
    void DrawLights();
    
    // Setters
    // ----------------------------------------
    void DefineShadowProperties(const std::shared_ptr<Material>& material);
    
    // Scene variables
    // ----------------------------------------
private:
    ///< Rendering camera.
    std::shared_ptr<Camera> m_Camera;
    
    ///< Light sources in the scene.
    LightLibrary m_Lights;
    ///< Set of objects in the scene.
    ModelLibrary m_Models;
    
    ///< Framebuffer(s) library with all the rendered images.
    FrameBufferLibrary m_FrameBuffers;
    
    ///< Viewport (displays the rendered image).
    std::shared_ptr<Viewport> m_Viewport;
    
    ///< Render passes for the rendering of the scene.
    RenderPassLibrary m_RenderPasses;
};

} // namespace pixc
