#pragma once

#include "Engine.h"

#include <glm/glm.hpp>

/**
 * @brief Rendering layer containing a basic 3D viewer.
 *
 * The `Viewer` class is a derived class of the `Layer` class and represents a rendering layer
 * specifically designed for a basic 3D viewer. It provides functionality for attaching, detaching, updating,
 * and handling events specific to the layer.
 *
 * Copying or moving `Viewer` objects is disabled to ensure single ownership and prevent
 * unintended layer duplication.
 */
class Viewer : public Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    Viewer(int width, int height);
    
    // Layer handlers
    // ----------------------------------------
    void OnAttach() override;
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    
    // Setters(s)
    // ----------------------------------------
    /// @brief Set the interaction state inside this layer.
    /// @param e Enable/disable the interaction.
    void EnableInteraction(const bool e) { m_Scene->GetCamera()->Enable(e); }
    
private:
    // Initialization
    // ----------------------------------------
    void InitializeViewer();
    
    void DefineMaterials();
    void DefineSceneGeometry();
    void DefineRenderPasses();
    
    // Events handler(s)
    // ----------------------------------------
    bool OnWindowResize(WindowResizeEvent &e);
    
    // Viewer layer variables
    // ----------------------------------------
private:
    ///< Scene to be rendered.
    std::unique_ptr<Scene> m_Scene;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Viewer);
};
