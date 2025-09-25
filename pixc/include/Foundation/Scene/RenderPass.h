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
 * @brief Represents the specification for a render pass in a rendering pipeline.
 *
 * The `RenderPassSpecification` struct defines all parameters and settings
 * for a single render pass, including camera, framebuffer, models, viewport,
 * clear settings, and optional pre/post rendering hooks.
 */
struct RenderPassSpecification
{
    ///< Whether this render pass is active.
    bool Active = true;
    ///< Human-readable name for this render pass.
    std::string Name;

    ///< The camera used to render this pass. If null, the scene's active camera is used.
    std::shared_ptr<Camera> Camera;

    /**
     * @brief Represents a single model and its associated material to be rendered.
     */
    struct Renderable
    {
        std::string ModelName;    ///< Name of the model in the scene's model library.
        std::string MaterialName; ///< Name of the material to use for this model.
        
        ///< Lambda to configure material properties before rendering this model.
        std::function<void(const std::shared_ptr<Material>& material)> MaterialSetupFunction;
    };
    ///< Collection of models to render in this pass.
    std::vector<Renderable> Models;
    
    ///< Whether to render the scene's lights in this pass.
    bool RenderLights = false;

    ///< The framebuffer to render to. If null, the default framebuffer is used.
    std::shared_ptr<FrameBuffer> FrameBuffer;

    /**
     * @brief Clear behavior for this render pass.
     *
     * Default behavior clears the framebuffer using the specified ClearColor.
     */
    enum class ClearMode { Enabled, Disabled, Default };
    ClearMode ClearBehavior = ClearMode::Enabled;
    ///< Color used to clear the framebuffer (only if ClearBehavior != Disabled).
    glm::vec4 ClearColor = glm::vec4(0.0f);

    ///< Optional override for the viewport size for this pass. Uses the scene's viewport if not specified.
    std::optional<glm::vec2> ViewportSize;

    ///< Optional lambda executed before rendering this pass.
    std::function<void()> PreRenderCode;
    ///< Optional lambda executed after rendering this pass.
    std::function<void()> PostRenderCode;

    // ----------------------------------------
    // Helper Constructors / Methods
    // ----------------------------------------
    RenderPassSpecification() = default;
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
