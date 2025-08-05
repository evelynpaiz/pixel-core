#pragma once

#include <pixc.h>

/**
 * @brief Example implementation of a basic rendering layer.
 *
 * The `SimpleLayer` class derives from `pixc::RenderingLayer` and demonstrates a minimal setup for
 * rendering in the Pixel Core engine. It overrides key lifecycle methods to define framebuffers, materials, lights,
 * and geometry specific to a simple rendering example.
 *
 * Copying or moving `SimpleLayer` objects is disabled to ensure single ownership and prevent unintended
 * duplication.
 */
class SimpleLayer : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    SimpleLayer(int width, int height, const std::string& name = "Simple Layer Example");
    /// @brief Delete the rendering layer.
    virtual ~SimpleLayer() = default;
    
    // Layer handlers
    // ----------------------------------------
    void OnUpdate(pixc::Timestep ts) override;
    
private:
    // Initialization
    // ----------------------------------------
    void DefineBuffers() override;
    void DefineMaterials() override;
    void DefineLights() override;
    void DefineGeometry() override;
    
    // Events handler(s)
    // ----------------------------------------
    bool OnWindowResize(pixc::WindowResizeEvent &e) override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SimpleLayer);
};
