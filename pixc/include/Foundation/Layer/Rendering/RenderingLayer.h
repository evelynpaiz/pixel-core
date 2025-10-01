#pragma once

#include "Foundation/Layer/Layer.h"

#include "Foundation/Event/WindowEvent.h"

#include "Foundation/Renderer/Camera/Camera.h"
#include "Foundation/Renderer/Light/Light.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

#include "Foundation/Scene/Viewport.h"

#include "Foundation/Scene/Scene.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Base class for rendering layers within the application.
 *
 * The `RenderingLayer` class extends the `Layer` system to define a structured and extensible interface
 * for managing rendering logic in the application. It provides a high-level setup for rendering scenes, including the
 * definition of frame buffers, materials, lights, and geometry.
 *
 * Subclasses should override the `DefineBuffers()`, `DefineMaterials()`, `DefineLights()`,
 * and `DefineGeometry()` methods to configure the rendering pipeline for specific use cases.
 *
 * Copying or moving `RenderingLayer` objects is disabled to ensure single ownership and prevent unintended
 * duplication.
 */
class RenderingLayer : public Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Create a rendering layer.
    /// @param width Width of the window where the layer is presented.
    /// @param height Height of the window where the layer is presented.
    /// @param name Name of the layer.
    RenderingLayer(const uint32_t width, const uint32_t height,
                   const std::string& name = "Rendering Layer")
        : Layer(name), m_Scene(width, height) {}
    /// @brief Delete the rendering layer.
    virtual ~RenderingLayer() = default;
    
    // Layer handlers
    // ----------------------------------------
    /// @brief Called when the rendering layer is attached to the application.
    void OnAttach() override { Initialize(); }
    /// @brief Handle an event that possibly occurred inside the viewer layer.
    /// @param e Event.
    void OnEvent(Event& e) override
    {
        // Define the event dispatcher
        EventDispatcher dispatcher(e);
        // Dispatch the event to the application event callbacks
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(RenderingLayer::OnWindowResize));
        
        // Handle the events on the camera
        if (m_Scene.GetCamera() && !e.Handled)
            m_Scene.GetCamera()->OnEvent(e);
    }
    
protected:
    // Initialization
    // ----------------------------------------
    /// @brief Define and configure frame buffers used in the rendering pipeline.
    virtual void DefineBuffers() {}
    /// @brief Define and register materials used for rendering.
    virtual void DefineMaterials() {}
    /// @brief Set up lighting parameters and light sources.
    virtual void DefineLights() {}
    /// @brief Define the geometry used in the scene.
    virtual void DefineGeometry() {}
    
    /// @brief Defines the rendering passes.
    virtual void DefineRenderPasses() {}
    
    /// @brief Initialize all resources required for rendering.
    virtual void Initialize()
    {
        // Define the frame bufer(s)
        DefineBuffers();
        // Define the material(s)
        DefineMaterials();
        // Define the light source(s)
        DefineLights();
        // Define the mesh(es) & model(s)
        DefineGeometry();
        
        // Define all the rendering passes
        DefineRenderPasses();
    }
    
    // Events handler(s)
    // ----------------------------------------
    /// @brief Function to be called when a window resize event happens.
    /// @param e Event to be handled.
    /// @return True if the event has been handled.
    virtual bool OnWindowResize(WindowResizeEvent &e)
    {
        // Notify of the change
        PIXEL_CORE_TRACE("Window resized to {0} x {1}", e.GetWidth(), e.GetHeight());
        
        // Update the camera size
        if (m_Scene.GetCamera())
            m_Scene.GetCamera()->SetViewportSize(e.GetWidth(), e.GetHeight());
        // Update the size of the viewport
        if (m_Scene.GetViewport())
            m_Scene.GetViewport()->Resize(e.GetWidth(), e.GetHeight());
        
        // Define the event as handled
        return true;
    }
    
    // Rendering layer variables
    // ----------------------------------------
protected:
    ///< Rendering scene.
    Scene m_Scene;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(RenderingLayer);
};

} // namespace pixc
