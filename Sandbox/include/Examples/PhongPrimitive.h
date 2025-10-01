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
class PhongPrimitive : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    PhongPrimitive(const uint32_t width, const uint32_t height,
                   const std::string& name = "Phong Primitive Layer");
    /// @brief Delete the rendering layer.
    virtual ~PhongPrimitive() = default;
    
    // Layer handlers
    // ----------------------------------------
    void OnUpdate(pixc::Timestep ts) override;
    
private:
    // Initialization
    // ----------------------------------------
    void DefineMaterials() override;
    void DefineLights() override;
    void DefineGeometry() override;
    
    void DefineRenderPasses() override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(PhongPrimitive);
};
