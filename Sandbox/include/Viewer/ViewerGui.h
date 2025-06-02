#pragma once

#include "Engine.h"

#include "Viewer/Viewer.h"

/**
 * @brief Rendering layer responsible for the graphics interface using the ImGui library.
 *
 * The `ViewerGui` class is a derived class of the `GuiLayer` class and represents a graphical
 * user interface (GUI) to provide graphical support to a 3D viewer. It offers functionality for attaching,
 * detaching, updating, and handling events specific to the layer.
 *
 * Copying or moving `ViewerGui` objects is disabled to ensure single ownership and prevent
 * unintended layer duplication.
 */
class ViewerGui : public pixc::GuiLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    ViewerGui(const std::shared_ptr<Viewer>& layer);
    
    // Layer handlers
    // ----------------------------------------
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    
private:
    // GUI
    // ----------------------------------------
    void GUIMenu();
    
    // Getter(s)
    // ----------------------------------------
    bool IsActive();
    
    // Setter(s)
    // ----------------------------------------
    void SetStyle() override;
    
    // Events handler(s)
    // ----------------------------------------
    bool OnMouseScrolled(MouseScrolledEvent &e);
    
    // GUI layer variables
    // ----------------------------------------
private:
    ///< Rendering layer.
    std::shared_ptr<Viewer> m_Viewer;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(ViewerGui);
};
