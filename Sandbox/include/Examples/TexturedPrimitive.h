#pragma once

#include <pixc.h>

class TexturedPrimitive : public pixc::RenderingLayer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    TexturedPrimitive(const uint32_t width, const uint32_t height,
                      const std::string& name = "Textured Primitive Layer");
    /// @brief Delete the rendering layer.
    virtual ~TexturedPrimitive() = default;
    
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
    DISABLE_COPY_AND_MOVE(TexturedPrimitive);
};
