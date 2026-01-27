#pragma once

#include <pixc.h>

/**
 * @brief Basic rendering sample.
 *
 * The `UnlitSample` class derives from `pixc::RenderingLayer` and demonstrates rendering
 * a loaded model using an unlit material without lighting calculations.
 *
 * Copying or moving `UnlitSample` objects is disabled to ensure single ownership
 * of the internal resources and prevent unintended duplication.
 */
class UnlitSample : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    UnlitSample(const uint32_t width, const uint32_t height,
                      const std::string& name = "Unlit Sample Layer");
    /// @brief Delete the rendering layer.
    virtual ~UnlitSample() = default;
    
    // Layer handlers
    // ----------------------------------------
    void OnUpdate(pixc::Timestep ts) override;
    
private:
    // Initialization
    // ----------------------------------------
    void DefineMaterials() override;
    void DefineGeometry() override;
    
    void DefineRenderPasses() override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(UnlitSample);
};
