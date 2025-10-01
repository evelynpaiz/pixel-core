#pragma once

#include "Foundation/Renderer/Camera/Camera.h"
#include "Foundation/Renderer/Buffer/FrameBuffer.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Target output of the render pass (framebuffer, viewport, and clear behavior).
 */
struct TargetSettings
{
    std::shared_ptr<FrameBuffer> FrameBuffer;           ///< Target framebuffer (null = default).
    std::optional<glm::vec2> ViewportSize;              ///< Viewport override (uses framebuffer or scene's viewport if not set).

    bool ClearEnabled = true;                           ///< Whether the framebuffer should be cleared.
    glm::vec4 ClearColor = glm::vec4(0.0f);             ///< Clear color (used only if ClearEnabled).
    std::optional<RenderTargetMask> ClearTargets;       ///< Buffers to clear (color, depth, stencil).
};

/**
 * @brief A single model to be rendered in a pass.
 */
struct Renderable
{
    std::string ModelName;                      ///< Name of the model in the scene's model library.
    std::string MaterialName;                   ///< Name of the material to use for this model.

    ///< Lambda to configure material properties before rendering this model.
    std::function<void(const std::shared_ptr<Material>&)> MaterialSetupFunction;
};

/**
 * @brief What is rendered in this pass (models, lights, camera).
 */
struct RenderSettings
{
    std::shared_ptr<Camera> Camera;             ///< Camera used for rendering (falls back to scene camera if null).
    std::vector<Renderable> Models;             ///< Models to render in this pass.
    bool RenderLights = false;                  ///< Whether to render lights in this pass.
};

/**
 * @brief User-defined callbacks executed around rendering.
 */
struct PassHooks
{
    std::function<void()> PreRenderCode;        ///< Lambda executed before rendering.
    std::function<void()> PostRenderCode;       ///< Lambda executed after rendering.
};

/**
 * @brief Represents the specification for a render pass in a rendering pipeline.
 */
struct RenderPassSpecification
{
    bool Active = true;                         ///< Whether this render pass is active.
    TargetSettings Target;                      ///< Framebuffer, viewport, clear.
    RenderSettings Render;                      ///< Models, camera, lights.
    PassHooks Hooks;                            ///< Custom code hooks.
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

} // namespace pixc
