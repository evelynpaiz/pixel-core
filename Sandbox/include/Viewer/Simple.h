#pragma once

#include "Engine.h"

#include <glm/glm.hpp>

class Simple : public Layer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    Simple(int width, int height);
    
    // Layer handlers
    // ----------------------------------------
    void OnAttach() override;
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;
    
private:
    // Events handler(s)
    // ----------------------------------------
    bool OnWindowResize(WindowResizeEvent &e);
    
private:
    ///< Rendering camera.
    std::shared_ptr<PerspectiveCamera> m_Camera;
    ///< Set of objects in the scene.
    ModelLibrary m_Models;
    
    std::shared_ptr<FrameBuffer> m_Framebuffer;
    std::shared_ptr<FrameBuffer> m_Final;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Simple);
};
