#include "Examples/PhongPrimitive.h"

/**
 * @brief Create a rendering layer.
 *
 * @param width Width of the window where the layer is presented.
 * @param height Height of the window where the layer is presented.
 * @param name Name of the layer.
 */
PhongPrimitive::PhongPrimitive(const uint32_t width, const uint32_t height,
                               const std::string& name)
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
void PhongPrimitive::DefineMaterials()
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
void PhongPrimitive::DefineLights()
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
    positional->SetSpecularStrength(0.6f);
    positional->GetModel()->SetScale(glm::vec3(0.05f));
    
    m_Scene.GetLights().Add("Positional", positional);
    
    // Define the directional light sources
    auto directional = std::make_shared<pixc::DirectionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    directional->InitShadowFrameBuffer(width, height);
    
    directional->SetDiffuseStrength(0.4f);
    directional->SetSpecularStrength(0.1f);
    
    m_Scene.GetLights().Add("Directional", directional);
}

/**
 * @brief Define the geometry used in the scene.
 */
void PhongPrimitive::DefineGeometry()
{
    // Define the model(s)
    auto sphere = pixc::utils::geometry::ModelSphere<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    sphere->SetScale(glm::vec3(0.3f));
    sphere->SetPosition(glm::vec3(-0.5f, 0.0f, 0.0f));
    m_Scene.GetModels().Add("Sphere", sphere);
    
    auto cube = pixc::utils::geometry::ModelCube<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(0.5f));
    cube->SetPosition(glm::vec3(0.5f, 0.0f, 0.0f));
    m_Scene.GetModels().Add("Cube", cube);
}

/**
 * @brief Defines the rendering passes.
 */
void PhongPrimitive::DefineRenderPasses()
{
    auto& library = m_Scene.GetRenderPasses();
    
    // First pass(es): shadow(s)
    //--------------------------------
    // TODO: remove from this example and move to a shadowed one
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
            { "Sphere", "Depth" },
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
        { "Cube", "Simple",
            [](const std::shared_ptr<pixc::Material>& material)
            {
                auto simpleMaterial =  std::dynamic_pointer_cast<pixc::SimpleMaterial>(material);
                
                if (!simpleMaterial)
                    return;

                static auto ground = pixc::Texture2D::CreateFromFile(pixc::ResourcesManager::SpecificPath("textures/sample/container.jpg"));
                simpleMaterial->SetColor(glm::vec4(1.0f));
                simpleMaterial->SetTextureMap(ground);
            }
        },
        { "Sphere", "PhongColor",
            [](const std::shared_ptr<pixc::Material>& material)
            {
                auto phongMaterial =  std::dynamic_pointer_cast<pixc::PhongColorMaterial>(material);
                
                if (!phongMaterial)
                    return;
                
                phongMaterial->SetAmbientColor(glm::vec3(0.8f, 0.2f, 0.4f));
                phongMaterial->SetDiffuseColor(glm::vec3(0.8f, 0.2f, 0.4f));
                phongMaterial->SetSpecularColor(glm::vec3(1.0f));
                
                // In a cube, the shininnes is discrete, using 100–300
                // and i a sphere, the shinines is smooth, using 8–32
                phongMaterial->SetShininess(8.0f);
            }
        }
    };
    scenePassSpec.Render.RenderLights = true;
    library.Add("Scene", scenePassSpec);
}

/**
 * @brief Render the rendering layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void PhongPrimitive::OnUpdate(pixc::Timestep ts)
{
    // Reset per-frame rendering statistics (e.g., draw calls, triangles, vertices)
    pixc::Renderer::ResetStats();
    
    // Draw all objects contained in the scene into its assigned viewport
    m_Scene.Draw();
    // Render the scene’s viewport content to the screen framebuffer
    m_Scene.GetViewport()->RenderToScreen();
    
    // Update the camera state (e.g., input handling, matrices) using the timestep
    m_Scene.GetCamera()->OnUpdate(ts);
}
