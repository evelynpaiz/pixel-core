#pragma once

#include <pixc.h>

/**
 * @brief Phong lighting sample.
 *
 * The `PhongSample` class derives from `pixc::RenderingLayer` and demonstrates forward
 * rendering using the Phong lighting model.
 *
 * Copying or moving `PhongSample` objects is disabled to ensure single ownership
 * of the internal resources and prevent unintended duplication.
 */
class PhongSample : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    PhongSample(const uint32_t width, const uint32_t height,
                const std::string& name = "Phong Sample Layer");
    /// @brief Delete the rendering layer.
    virtual ~PhongSample() = default;
    
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
    DISABLE_COPY_AND_MOVE(PhongSample);
};
