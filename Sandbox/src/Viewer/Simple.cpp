#include "Viewer/Simple.h"

#include "Common/Renderer/RendererCommand.h"
#include "Common/Renderer/Shader/Shader.h"
#include "Common/Renderer/Material/Material.h"

#include <glm/gtc/matrix_transform.hpp>

/**
 * Define a layer for a 3D viewer.
 */
Simple::Simple(int width, int height)
    : Layer("Metal Testing Layer")
{
    // Define the rendering camera
    m_Camera = std::make_shared<PerspectiveCamera>(width, height);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
}

/**
 * Attach (add) the viewer layer to the rendering engine.
 */
void Simple::OnAttach()
{
    // Define the material(s)
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    auto simple = materialLibrary.Create<SimpleMaterial>("Simple");
    
    // Define the model(s)
    auto cube = utils::Geometry::ModelCube<GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(2.0f));
    cube->SetMaterial(simple);
    m_Models.Add("Cube", cube);
    
    auto plane = utils::Geometry::ModelPlane<GeoVertexData<glm::vec4, glm::vec2>>();
    plane->SetPosition(glm::vec3(0.0f, -1.5f, 0.0f));
    plane->SetScale(glm::vec3(10.0f));
    plane->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    plane->SetMaterial(simple);
    m_Models.Add("Plane", plane);
}

/**
 * Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void Simple::OnUpdate(Timestep ts)
{
    // Define rendering texture(s)
    static auto &white = utils::textures::WhiteTexture2D();
    static auto container = Texture2D::CreateFromFile("Resources/textures/container.jpg");
    
    // Get the material(s)
    auto material = std::dynamic_pointer_cast<SimpleMaterial>(
                      Renderer::GetMaterialLibrary().Get("Simple"));
    
    // Get the model(s)
    auto& cube = m_Models.Get("Cube");
    auto& plane = m_Models.Get("Plane");
    
    // Reset rendering statistics
    Renderer::ResetStats();
    
    // Set the target to render into
    RendererCommand::SetViewport(0, 0, m_Camera->GetWidth(), m_Camera->GetHeight());
    RendererCommand::SetRenderTarget(glm::vec4(0.93f, 0.93f, 0.93f, 1.0f),
                                     {true, true, false});
    // Render the scene
    Renderer::BeginScene(m_Camera);
    
    material->SetColor(glm::vec4(0.3f, 0.0f, 0.7f, 1.0f));
    material->SetTextureMap(white);
    cube->DrawModel();
    
    material->SetColor(glm::vec4(1.0f));
    material->SetTextureMap(container);
    plane->DrawModel();
    
    Renderer::EndScene();
    
    // Update camera
    m_Camera->OnUpdate(ts);
}

/**
 * Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void Simple::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
        BIND_EVENT_FN(Simple::OnWindowResize));
    
    // Handle the events on the camera
    m_Camera->OnEvent(e);
}

/**
 * Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool Simple::OnWindowResize(WindowResizeEvent &e)
{
    // Update the camera
    m_Camera->SetViewportSize(e.GetWidth(), e.GetHeight());
    
    return true;
}
