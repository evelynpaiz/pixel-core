#pragma once

#include "Foundation/Layer/Layer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

class WindowResizeEvent;

class RenderingLayer : public Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    RenderingLayer(int width, int height, const std::string& name = "Rendering Layer");
    /// @brief Delete the rendering layer.
    virtual ~RenderingLayer() = default;
    
    // Layer handlers
    // ----------------------------------------
    void OnAttach() override;
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    
private:
    // Events handler(s)
    // ----------------------------------------
    virtual bool OnWindowResize(WindowResizeEvent &e);
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(RenderingLayer);
};

} // namespace pixc
