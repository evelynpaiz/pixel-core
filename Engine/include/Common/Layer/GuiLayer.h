#pragma once

#include "Common/Layer/Layer.h"

struct ImGuiContext;

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Rendering layer responsible for the graphics interface using the ImGui library.
 *
 * The `GuiLayer` class is a derived class of the `Layer` class and represents a graphical
 * user interface (GUI) to provide graphical support to the user. It offers functionality for attaching,
 * detaching, updating, and handling events specific to the layer.
 *
 * Copying or moving `GuiLayer` objects is disabled to ensure single ownership and prevent
 * unintended layer duplication.
 */
class GuiLayer : public Layer
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    GuiLayer(const std::string& name = "GUI Layer");
    
    // Layer handlers
    // ----------------------------------------
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    
    // Layer rendering
    // ----------------------------------------
    void Begin();
    void End();
    
    // Event handler
    // ----------------------------------------
    /// @brief Dispatch the events only to this layer.
    /// @param block Block the dispatching of the events.
    void BlockEvents(bool block) { m_BlockEvents = block; }
    
    protected:
    // GUI
    // ----------------------------------------
    void GUIStats(Timestep ts);
    
    // Setter(s)
    // ----------------------------------------
    virtual void SetStyle();
    
    // GUI layer variables
    // ----------------------------------------
    private:
    ///< GUI context (using ImGui)
    ImGuiContext *m_GuiContext = nullptr;
    ///< Dispatch the event to this layers only.
    bool m_BlockEvents = true;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(GuiLayer);
};

} // namespace pixc
