#pragma once

#include "Common/Renderer/Light/Light.h"
#include "Common/Renderer/Light/EnvironmentLight.h"

#include "Common/Scene/Viewport.h"

/**
 * Represents the specification for a render pass in a rendering pipeline.
 *
 * The `RenderPassSpecification` struct defines the parameters and settings
 * for a single render pass in a rendering pipeline. It includes information
 * about the camera, framebuffer, models to be rendered, and optionally the
 * clear color for the framebuffer.
 */
struct RenderPassSpecification
{
    ///< Active flag.
    bool Active = true;
    
    ///< The camera used for rendering in this pass.
    std::shared_ptr<Camera> Camera;
    ///< The models to render in this pass, along with their associated materials.
    std::unordered_multimap<std::string, std::string> Models;
    ///< The framebuffer to render to in this pass.
    std::shared_ptr<FrameBuffer> Framebuffer;
    
    ///< The clear color for the framebuffer, if specified.
    std::optional<glm::vec4> Color;
    ///< The viewport size to render into, if specified.
    std::optional<glm::vec2> Size;
    ///< Disable the clearing of the framebuffer (or screenbuffer), if specified.
    std::optional<bool> SkipClear;
    
    ///< Optional piece of code to be executed after rendering the pass.
    std::function<void()> PreRenderCode;
    ///< Optional piece of code to be executed after rendering the pass.
    std::function<void()> PostRenderCode;
};

/**
 * A library for managing render passes used in rendering.
 *
 * The `RenderPassLibrary` class provides functionality to add, create, retrieve, and check for
 * the existence of render passes within the library. Render passes can be associated with unique names
 * for easy access.
 */
class RenderPassLibrary : public Library<RenderPassSpecification>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new model library.
    RenderPassLibrary() : Library("Render pass") {}
    
    // Constructor/Destructor
    // ----------------------------------------
    /// @brief Delete the library.
    ~RenderPassLibrary() override = default;
    
    // Add/Load
    // ----------------------------------------
    /// @brief Adds an object to the library.
    /// @param name The name to associate with the object.
    /// @param object The object to add.
    /// @note If an object with the same name already exists in the library, an assertion failure
    /// will occur.
    void Add(const std::string& name,
             const RenderPassSpecification& object) override
    {
        Library::Add(name, object);
        m_Order.push_back(name);
    }
    
    // Library variables
    // ----------------------------------------
private:
    ///< Rendering order.
    std::vector<std::string> m_Order;
    
    // Friend classes
    // ----------------------------------------
public:
    friend class Scene;
};

class Scene
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Create a new scene.
    Scene(int width, int height,
          const std::filesystem::path& viewportShader = "");
    /// @brief Delete the specified scene.
    ~Scene() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the size (width) of the viewport.
    /// @return The width size.
    int GetViewportWidth() const { return m_Viewport->m_Width; };
    /// @brief Get the size (height) of the viewport.
    /// @return The height size.
    int GetViewportHeight() const { return m_Viewport->m_Height; };
    
    /// @brief Get the viewport where the scene is being rendered.
    /// @return The viewport.
    const std::shared_ptr<Viewport>& GetViewport() const { return m_Viewport; }
    
    /// @brief Get the camera used currently to render the scene.
    /// @return The active camera.
    const std::shared_ptr<Camera>& GetCamera() const { return m_Camera; }
    /// @brief Get the light sources defined in the scene.
    /// @return The scene's lights.
    LightLibrary& GetLightSouces() { return m_Lights; }
    /// @brief Get the models defined in the scene.
    /// @return The scene's models.
    ModelLibrary& GetModels() { return m_Models; }
    
    /// @brief Get the framebuffers library.
    /// @return The defined framebuffers.
    FrameBufferLibrary& GetFrameBufferLibrary() { return m_FramebufferLibrary; }
    
    /// @brief Get the render passes on the scene.
    /// @return The defined render passes with its specifications.
    RenderPassLibrary& GetRenderPasses() { return m_RenderPasses; }
    
    // Render
    // ----------------------------------------
    void Draw();
    
private:
    void Draw(const RenderPassSpecification& pass);
    void DrawLight();
    
    // Setters
    // ----------------------------------------
    void DefineShadowProperties(const std::shared_ptr<Material>& material);
    
    // Scene variables
    // ----------------------------------------
private:
    ///< Viewport (displays the rendered image).
    std::shared_ptr<Viewport> m_Viewport;
    
    ///< Main camera.
    std::shared_ptr<Camera> m_Camera;
    ///< Light sources in the scene.
    LightLibrary m_Lights;
    ///< Set of objects in the scene.
    ModelLibrary m_Models;
    
    ///< Framebuffer(s) library with all the rendered images.
    FrameBufferLibrary m_FramebufferLibrary;
    
    ///< Render passes for the rendering of the scene.
    RenderPassLibrary m_RenderPasses;
};
