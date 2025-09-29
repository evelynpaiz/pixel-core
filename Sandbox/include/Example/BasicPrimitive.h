#pragma once

#include <pixc.h>

/**
 * @brief Example implementation of a basic primitive rendering layer.
 *
 * The `BasicPrimitive` class derives from `pixc::Layer` and provides a simple example
 * of rendering a geometric primitive in the Pixel Core engine. It manages its own viewport
 * size, material, and model, and overrides the key lifecycle methods to handle attachment,
 * updating per frame, and responding to window resize events.
 *
 * Copying or moving `BasicPrimitive` objects is disabled to ensure single ownership
 * of the internal resources and prevent unintended duplication.
 */
class BasicPrimitive : public pixc::Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    BasicPrimitive(int width, int height,
                   const std::string& name = "Basic Primitive Layer")
        : m_Width(width), m_Height(height) {}
    /// @brief Delete the rendering layer.
    virtual ~BasicPrimitive() = default;
    
    // Layer handlers
    // ----------------------------------------
    void OnAttach() override;
    void OnUpdate(pixc::Timestep ts) override;
    void OnEvent(pixc::Event& e) override;
    
    // Events handler(s)
    // ----------------------------------------
    bool OnWindowResize(pixc::WindowResizeEvent &e);
    
private:
    ///< Viewport size.
    uint32_t m_Width, m_Height;
    
    ///< Model to be rendered.
    std::shared_ptr<pixc::BaseModel> m_Model;
    ///< Material to be used for rendering.
    std::shared_ptr<pixc::SimpleColorMaterial> m_Material;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(BasicPrimitive);
};
