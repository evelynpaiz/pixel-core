#pragma once

#include "Foundation/Core/ClassUtils.h"

#include "Foundation/Layer/Layer.h"

#include "Foundation/Renderer/Camera/Camera.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

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
    
    // Rendering layer variables
    // ----------------------------------------
private:
    ///< Rendering camera.
    std::shared_ptr<Camera> m_Camera;
    ///< Set of objects in the scene.
    ModelLibrary m_Models;
    
    struct VertexData
    {
        glm::vec4 position;         ///< Vertex position.
        glm::vec2 uv;               ///< Texture coordinate.
    };
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(RenderingLayer);
};

} // namespace pixc
