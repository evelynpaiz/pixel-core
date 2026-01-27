#pragma once

#include <pixc.h>

/**
 * @brief Unlit rendering sample.
 *
 * The `BasicSample` class derives from `pixc::Layer` and demonstrates the most
 * minimal setup required to render a simple geometric primitive in the Pixel Core engine.
 *
 * Copying or moving `BasicSample` objects is disabled to ensure single ownership
 * of the internal resources and prevent unintended duplication.
 */
class BasicSample : public pixc::Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Create a rendering layer.
    /// @param width Width of the window where the layer is presented.
    /// @param height Height of the window where the layer is presented.
    /// @param name Name of the layer.
    BasicSample(const uint32_t width, const uint32_t height,
                const std::string& name = "Basic Sample Layer")
        : Layer(name), m_Width(width), m_Height(height) {}
    /// @brief Delete the rendering layer.
    virtual ~BasicSample() = default;
    
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
    std::shared_ptr<pixc::UnlitMaterial> m_Material;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(BasicSample);
};
