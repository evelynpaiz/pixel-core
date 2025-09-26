#include "Layer/SimpleLayer.h"

/**
 * @brief Define a layer for a 3D viewer.
 */
SimpleLayer::SimpleLayer(int width, int height, const std::string& name)
    : RenderingLayer(width, height, name)
{
    // Define the rendering camera
    m_Scene.GetCamera()->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    
    // Update the subsampling scale of the viewport
    m_Scene.GetViewport()->ReScale(2.0f);
}

/**
 * @brief Define and register materials used for rendering.
 */
void SimpleLayer::DefineMaterials()
{
    // Get the material library defined in the renderer
    auto& materialLibrary = pixc::Renderer::GetMaterialLibrary();
    
    // Define the new material(s)
    materialLibrary.Create<pixc::SimpleMaterial>("Simple");
    materialLibrary.Create<pixc::PhongColorMaterial>("PhongColor");
}

/**
 * @brief Set up lighting parameters and light sources.
 */
void SimpleLayer::DefineLights()
{
    uint32_t width = m_Scene.GetCamera()->GetWidth();
    uint32_t height = m_Scene.GetCamera()->GetHeight();
    
    // Define the environment light
    auto environment = std::make_shared<pixc::EnvironmentLight>();
    m_Scene.GetLights().Add("Environment", environment);
    
    // Define the positional light source(s)
    auto positional = std::make_shared<pixc::PositionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    positional->InitShadowFrameBuffer(width, height);
    
    positional->SetDiffuseStrength(0.6f);
    positional->SetSpecularStrength(0.4f);
    positional->GetModel()->SetScale(glm::vec3(0.05f));
    
    m_Scene.GetLights().Add("Positional", positional);
    
    // Define the directional light sources
    auto directional = std::make_shared<pixc::DirectionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f), 1.0f, 2.0f);
    directional->InitShadowFrameBuffer(width, height);
    
    directional->SetDiffuseStrength(0.6f);
    directional->SetSpecularStrength(0.4f);
    
    m_Scene.GetLights().Add("Directional", directional);
}

/**
 * @brief Define the geometry used in the scene.
 */
void SimpleLayer::DefineGeometry()
{
    // Define the model(s)
    auto planet = std::make_shared<pixc::AssimpModel>(pixc::ResourcesManager::SpecificPath("models/sample/planet/planet.obj"));
    planet->SetScale(glm::vec3(0.12f));
    planet->SetPosition(glm::vec3(-0.5f, 0.0f, 0.0f));
    m_Scene.GetModels().Add("Planet", planet);
    
    auto cube = pixc::utils::geometry::ModelCube<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(0.5f));
    cube->SetPosition(glm::vec3(0.5f, 0.0f, 0.0f));
    m_Scene.GetModels().Add("Cube", cube);
}

/**
 * @brief Defines the rendering passes.
 */
void SimpleLayer::DefineRenderPasses()
{
    auto& library = m_Scene.GetRenderPasses();
    
    // First pass(es): shadow(s)
    //--------------------------------
    auto& lights = m_Scene.GetLights();
    for (auto& pair : lights)
    {
        auto light = std::dynamic_pointer_cast<pixc::LightCaster>(pair.second);
        if (!light)
            continue;
        
        pixc::RenderPassSpecification shadowPassSpec;
        shadowPassSpec.Target.FrameBuffer = light->GetShadowFrameBuffer();
        shadowPassSpec.Render.Camera = light->GetShadowCamera();
        shadowPassSpec.Render.Models = {
            { "Planet", "Depth" },
            { "Cube", "Depth" },
        };
        shadowPassSpec.Hooks.PreRenderCode = []() { pixc::RendererCommand::SetFaceCulling(pixc::FaceCulling::Front); };
        shadowPassSpec.Hooks.PostRenderCode = []() { pixc::RendererCommand::SetFaceCulling(pixc::FaceCulling::Back); };
    
        library.Add("Shadow-" + pair.first, shadowPassSpec);
    }
    
    // Second pass: scene
    //--------------------------------
    pixc::RenderPassSpecification scenePassSpec;
    scenePassSpec.Target.FrameBuffer = m_Scene.GetFrameBuffers().Get("ScreenBuffer");
    scenePassSpec.Target.ClearColor = glm::vec4(0.33f, 0.33f, 0.33f, 1.0f);
    scenePassSpec.Render.Camera = m_Scene.GetCamera();
    scenePassSpec.Render.Models = {
        { "Planet", "Simple",
            [](const std::shared_ptr<pixc::Material>& material)
            {
                auto simpleMaterial =  std::dynamic_pointer_cast<pixc::SimpleMaterial>(material);
                
                if (!simpleMaterial)
                    return;

                static auto ground = pixc::Texture2D::CreateFromFile(pixc::ResourcesManager::SpecificPath("models/sample/planet/planet_Quom1200.png"));
                simpleMaterial->SetColor(glm::vec4(1.0f));
                simpleMaterial->SetTextureMap(ground);
            }
        },
        { "Cube", "PhongColor",
            [](const std::shared_ptr<pixc::Material>& material)
            {
                auto phongMaterial =  std::dynamic_pointer_cast<pixc::PhongColorMaterial>(material);
                
                if (!phongMaterial)
                    return;
                
                phongMaterial->SetAmbientColor(glm::vec3(0.8f, 0.2f, 0.4f));
                phongMaterial->SetDiffuseColor(glm::vec3(0.8f, 0.2f, 0.4f));
                phongMaterial->SetSpecularColor(glm::vec3(1.0f));
                phongMaterial->SetShininess(100.0f);
            }
        }
    };
    scenePassSpec.Render.RenderLights = true;
    library.Add("Scene", scenePassSpec);
}

/**
 * @brief Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void SimpleLayer::OnUpdate(pixc::Timestep ts)
{
    // Reset rendering statistics
    pixc::Renderer::ResetStats();
    
    // Render
    m_Scene.Draw();
    m_Scene.GetViewport()->RenderToScreen();
    
    // Update camera
    m_Scene.GetCamera()->OnUpdate(ts);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool SimpleLayer::OnWindowResize(pixc::WindowResizeEvent &e)
{
    RenderingLayer::OnWindowResize(e);
    
    // Update specific size(s)
    m_Scene.GetViewport()->Resize(e.GetWidth(), e.GetHeight());
    
    // Define the event as handled
    return true;
}
