#pragma once

#include <pixc.h>

/**
 * @brief Depth visualization sample.
 *
 * The `DepthSample` class derives from `pixc::RenderingLayer` and demonstrates rendering
 * scene depth to a texture and visualizing it in a second pass.
 *
 * Copying or moving `DepthSample` objects is disabled to ensure single ownership
 * of the internal resources and prevent unintended duplication.
 */
class DepthSample : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    DepthSample(const uint32_t width, const uint32_t height,
                const std::string& name = "Depth Sample Layer");
    /// @brief Delete the rendering layer.
    virtual ~DepthSample() = default;
    
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
    DISABLE_COPY_AND_MOVE(DepthSample);
};
