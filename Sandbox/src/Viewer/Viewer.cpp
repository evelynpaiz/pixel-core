#include "Viewer/Viewer.h"

#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Define a layer for a 3D viewer.
 */
Viewer::Viewer(int width, int height)
    : Layer("Viewer Layer"), m_Scene(std::make_unique<Scene>(width, height))
{}

/**
 * @brief Attach (add) the viewer layer to the rendering engine.
 */
void Viewer::OnAttach()
{
    // Initialize all the elements of the viewer
    InitializeViewer();
}

/**
 * @brief Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void Viewer::OnUpdate(Timestep ts)
{
    // Reset rendering statistics
    Renderer::ResetStats();
    
    // Get the models in the scene
    auto models = m_Scene->GetModels();
    
    m_Scene->Draw();
    
    // Update the camera
    m_Scene->GetCamera()->OnUpdate(ts);
}

/**
 * @brief Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void Viewer::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
        BIND_EVENT_FN(Viewer::OnWindowResize));
    
    // Handle the events on the camera
    m_Scene->GetCamera()->OnEvent(e);
}

/**
 * @brief Initialize all components inside the viewer layer.
 */
void Viewer::InitializeViewer()
{
    int viewportWidth = m_Scene->GetViewport()->GetWidth();
    int viewportHeight = m_Scene->GetViewport()->GetHeight();
    
    // Define the environment and its light source
    auto positional = std::make_shared<PositionalLight>(viewportWidth, viewportHeight,
                                                        glm::vec3(1.0f), glm::vec3(3.0f, 5.0f, 0.0f));
    positional->SetDiffuseStrength(0.6f);
    positional->SetSpecularStrength(0.4f);
    m_Scene->GetLightSouces().Add("Positional", positional);
    
    auto directional = std::make_shared<DirectionalLight>(viewportWidth, viewportHeight,
                                                          glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    directional->SetDiffuseStrength(0.6f);
    directional->SetSpecularStrength(0.4f);
    m_Scene->GetLightSouces().Add("Directional", directional);
    
    // Update the position of the rendering camera
    m_Scene->GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    
    // Define the material(s) to be used for shading
    DefineMaterials();
    
    // Define the geometry in the scene
    DefineSceneGeometry();
    
    // Define the rendering passes during run-time
    DefineRenderPasses();
}

/**
 * @brief Defines the materials used in the rendering process.
 */
void Viewer::DefineMaterials()
{
    auto& library = Renderer::GetMaterialLibrary();
    
    auto cubeMaterial = library.Create<PhongTextureMaterial>("PhongTexture",
        "Resources/shaders/phong/PhongTextureShadow.glsl");
    cubeMaterial->SetDiffuseMap(Texture2D::CreateFromFile("Resources/textures/diffuse.jpeg"));
    cubeMaterial->SetSpecularMap(Texture2D::CreateFromFile("Resources/textures/specular.jpeg"));
    cubeMaterial->SetShininess(32.0f);
    
    auto planeMaterial = library.Create<PhongColorMaterial>("PhongColor",
        "Resources/shaders/phong/PhongColorShadow.glsl");
    planeMaterial->SetAmbientColor(glm::vec3(0.8f, 0.2f, 0.4f));
    planeMaterial->SetDiffuseColor(glm::vec3(0.8f, 0.2f, 0.4f));
    planeMaterial->SetSpecularColor(glm::vec3(1.0f));
    planeMaterial->SetShininess(100.0f);
}

/**
 * @brief Defines the scene geometry for the rendering process.
 */
void Viewer::DefineSceneGeometry()
{
    // Define the cube and plane model
    auto cube = utils::Geometry::ModelCube<GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(2.0f));
    m_Scene->GetModels().Add("Cube", cube);
    
    auto plane = utils::Geometry::ModelPlane<GeoVertexData<glm::vec4, glm::vec3>>();
    plane->SetPosition(glm::vec3(0.0f, -1.5f, 0.0f));
    plane->SetScale(glm::vec3(10.0f));
    plane->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    m_Scene->GetModels().Add("Plane", plane);
}

/**
 * @brief Defines the rendering passes.
 */
void Viewer::DefineRenderPasses()
{
    auto& library = m_Scene->GetRenderPasses();
    
    // First pass: shadows
    //--------------------------------
    auto& lights = m_Scene->GetLightSouces();
    for (auto& pair : lights)
    {
        auto light = std::dynamic_pointer_cast<Light>(pair.second);
        if (!light)
            continue;
        
        RenderPassSpecification shadowPassSpec;
        shadowPassSpec.Camera = light->GetShadowCamera();
        shadowPassSpec.Framebuffer = light->GetFramebuffer();
        shadowPassSpec.Models = {
            { "Cube", "Depth"},
            { "Plane", "Depth" },
        };
        shadowPassSpec.PreRenderCode = []() { Renderer::SetFaceCulling(FaceCulling::Front); };
        shadowPassSpec.PostRenderCode = []() { Renderer::SetFaceCulling(FaceCulling::Back); };
        
        library.Add("Shadow-" + pair.first, shadowPassSpec);
    }
    
    // Second pass: scene
    //--------------------------------
    RenderPassSpecification scenePassSpec;
    scenePassSpec.Camera = m_Scene->GetCamera();
    scenePassSpec.Framebuffer = m_Scene->GetViewport()->GetFramebuffer();
    scenePassSpec.Models = {
        { "Cube", "PhongTexture" },
        { "Plane", "PhongColor" },
        { "Light", "" }
    };
    scenePassSpec.Color = glm::vec4(0.93f, 0.93f, 0.93f, 1.0f);
    library.Add("Scene", scenePassSpec);
    
    RenderPassSpecification screenPassSpec;
    screenPassSpec.Models = { { "Viewport", "Viewport" } };
    screenPassSpec.Size = { m_Scene->GetViewportWidth(), m_Scene->GetViewportHeight() };
    library.Add("Viewport", screenPassSpec);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool Viewer::OnWindowResize(WindowResizeEvent &e)
{
    // Update the camera
    m_Scene->GetCamera()->SetViewportSize(e.GetWidth(), e.GetHeight());
    
    // Update the viewport
    m_Scene->GetViewport()->Resize(e.GetWidth(), e.GetHeight());
    m_Scene->GetRenderPasses().Get("Viewport").Size = { e.GetWidth(), e.GetHeight() };
    return true;
}
