#include "Foundation/Layer/Rendering/RenderingLayer.h"

#include "Foundation/Core/Resources.h"
#include "Foundation/Event/WindowEvent.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/RendererCommand.h"

#include "Foundation/Renderer/Camera/PerspectiveCamera.h"

#include "Foundation/Renderer/Texture/Texture2D.h"
#include "Foundation/Renderer/Material/SimpleMaterial.h"

#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"
#include "Foundation/Renderer/Drawable/Model/AssimpModel.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Foundation/Core/Resources.h"

namespace pixc {

/**
 * @brief Define a layer for a 3D viewer.
 */
RenderingLayer::RenderingLayer(int width, int height,
                               const std::string& name)
: Layer(name)
{
    // Define the rendering camera
    m_Camera = std::make_shared<PerspectiveCamera>(width, height);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
}

/**
 * @brief Attach (add) the viewer layer to the rendering engine.
 */
void RenderingLayer::OnAttach()
{
    // Define the material(s)
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    auto simple = materialLibrary.Create<SimpleMaterial>("Simple");
    
    // Define the model(s)
    auto cube = utils::geometry::ModelCube<GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(0.5f));
    cube->SetMaterial(simple);
    m_Models.Add("Cube", cube);
    
    auto plane = utils::geometry::ModelPlane<GeoVertexData<glm::vec4, glm::vec2>>();
    plane->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
    plane->SetScale(glm::vec3(2.0f));
    plane->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    plane->SetMaterial(simple);
    m_Models.Add("Plane", plane);
    
    auto planet = std::make_shared<AssimpModel>(ResourcesManager::SpecificPath("models/sample/planet/planet.obj"));
    planet->SetScale(glm::vec3(0.15f));
    planet->SetMaterial(simple);
    m_Models.Add("Planet", planet);
}

/**
 * @brief Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void RenderingLayer::OnUpdate(Timestep ts)
{
    // Define rendering texture(s)
    static auto &white = utils::textures::WhiteTexture2D();
    static auto container = Texture2D::CreateFromFile(ResourcesManager::SpecificPath("textures/sample/container.jpg"));
    static auto ground = Texture2D::CreateFromFile(ResourcesManager::SpecificPath("models/sample/planet/planet_Quom1200.png"));
    
    // Get the material(s)
    auto material = std::dynamic_pointer_cast<SimpleMaterial>(
                      Renderer::GetMaterialLibrary().Get("Simple"));
    
    // Get the model(s)
    auto& cube = m_Models.Get("Cube");
    auto& plane = m_Models.Get("Plane");
    auto& planet = m_Models.Get("Planet");
    
    // Reset rendering statistics
    Renderer::ResetStats();
    
    // Render
    RendererCommand::BeginRenderPass();
    RendererCommand::SetClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
    RendererCommand::Clear({true, true, false});
    
    Renderer::BeginScene(m_Camera);
    
    material->SetColor(glm::vec4(0.7f, 0.6f, 0.85f, 1.0f));
    material->SetTextureMap(ground);
    planet->DrawModel();
    
    material->SetColor(glm::vec4(1.0f));
    material->SetTextureMap(container);
    plane->DrawModel();
    
    Renderer::EndScene();

    RendererCommand::EndRenderPass();
    
    // Update camera
    m_Camera->OnUpdate(ts);
}

/**
 * @brief Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void RenderingLayer::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
                                           BIND_EVENT_FN(RenderingLayer::OnWindowResize));
    
    // Handle the events on the camera
    m_Camera->OnEvent(e);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool RenderingLayer::OnWindowResize(WindowResizeEvent &e)
{
    // Update the camera
    m_Camera->SetViewportSize(e.GetWidth(), e.GetHeight());
    
    //m_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
    
    PIXEL_CORE_TRACE("Window resized to {0} x {1}", e.GetWidth(), e.GetHeight());
    return true;
}

} // namespace pixc
