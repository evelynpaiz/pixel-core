#include "Layer/SimpleLayer.h"

/**
 * @brief Define a layer for a 3D viewer.
 */
SimpleLayer::SimpleLayer(int width, int height, const std::string& name)
// TODO: pass a scene initialized?
: RenderingLayer(width, height, name)
{
    // Define the rendering camera
    m_Camera = std::make_shared<pixc::PerspectiveCamera>(width, height);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
}

/**
 * @brief Define and configure frame buffers used in the rendering pipeline.
 */
void SimpleLayer::DefineBuffers()
{
    // Define a framebuffer specification
    pixc::FrameBufferSpecification spec;
    
    // Configure the specification for a super-sampling framebuffer
    spec.SetFrameBufferSize(m_Camera->GetWidth() * 2, m_Camera->GetHeight() * 2);
    spec.AttachmentsSpec = {
        { pixc::TextureType::TEXTURE2D, pixc::TextureFormat::RGBA8 },
        { pixc::TextureType::TEXTURE2D, pixc::TextureFormat::DEPTH16 }
    };
    
    m_Framebuffers.Create("Scene", spec);
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
    unsigned int width = m_Camera->GetWidth();
    unsigned int height = m_Camera->GetHeight();
    
    // Define the environment light
    auto environment = std::make_shared<pixc::EnvironmentLight>();
    environment->SetEnvironmentSize(60.0f);
    
    static auto envMap = pixc::Texture2D::CreateFromFile(pixc::ResourcesManager::SpecificPath("environment/env.hdr"));
    environment->SetEnvironmentMap(envMap);
    
    m_Lights.Add("Environment", environment);
    
    // Define the positional light source(s)
    auto positional = std::make_shared<pixc::PositionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    positional->InitShadowFrameBuffer(width, height);
    
    positional->SetDiffuseStrength(0.6f);
    positional->SetSpecularStrength(0.4f);
    positional->GetModel()->SetScale(glm::vec3(0.05f));
    
    m_Lights.Add("Positional", positional);
    
    // Define the directional light sources
    auto directional = std::make_shared<pixc::DirectionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f), 1.0f, 2.0f);
    directional->InitShadowFrameBuffer(width, height);
    
    directional->SetDiffuseStrength(0.6f);
    directional->SetSpecularStrength(0.4f);
    
    m_Lights.Add("Directional", directional);
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
    m_Models.Add("Planet", planet);
    
    auto cube = pixc::utils::geometry::ModelCube<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    cube->SetScale(glm::vec3(0.5f));
    cube->SetPosition(glm::vec3(0.5f, 0.0f, 0.0f));
    m_Models.Add("Cube", cube);
    
    auto viewport = pixc::utils::geometry::ModelPlane<pixc::GeoVertexData<glm::vec4, glm::vec2>>();
    viewport->SetScale(glm::vec3(2.0f));
    m_Models.Add("Viewport", viewport);
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
    
    // Define rendering texture(s)
    static auto &white = pixc::utils::textures::WhiteTexture2D();
    static auto container = pixc::Texture2D::CreateFromFile(pixc::ResourcesManager::SpecificPath("textures/sample/container.jpg"));
    static auto ground = pixc::Texture2D::CreateFromFile(pixc::ResourcesManager::SpecificPath("models/sample/planet/planet_Quom1200.png"));
    
    // Get the material(s)
    auto& depthMaterial = pixc::Renderer::GetMaterialLibrary().Get("Depth");
    
    auto simpleMaterial = std::dynamic_pointer_cast<pixc::SimpleMaterial>(
                          pixc::Renderer::GetMaterialLibrary().Get("Simple"));
    auto phongMaterial =  std::dynamic_pointer_cast<pixc::PhongColorMaterial>(
                          pixc::Renderer::GetMaterialLibrary().Get("PhongColor"));
    
    // Get the model(s)
    auto& planet = m_Models.Get("Planet");
    auto& cube = m_Models.Get("Cube");
    
    auto& viewport = m_Models.Get("Viewport");
    
    // Get the frame buffer(s)
    auto& sceneFB = m_Framebuffers.Get("Scene");
    
    // Render
    // -------
    for(auto& light : m_Lights)
    {
        auto caster = std::dynamic_pointer_cast<pixc::LightCaster>(light.second);
        
        if (!caster)
            continue;
        
        pixc::RendererCommand::BeginRenderPass(caster->GetShadowFramebuffer());
        pixc::RendererCommand::SetClearColor(glm::vec4(0.0f));
        pixc::RendererCommand::Clear();
        
        pixc::Renderer::BeginScene(caster->GetShadowCamera());
        
        planet->SetMaterial(depthMaterial);
        planet->DrawModel();
        
        cube->SetMaterial(depthMaterial);
        cube->DrawModel();
        
        pixc::Renderer::EndScene();

        pixc::RendererCommand::EndRenderPass();
    }
    
    // -------
    pixc::RendererCommand::BeginRenderPass(sceneFB);
    pixc::RendererCommand::SetClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
    pixc::RendererCommand::Clear();
    
    pixc::Renderer::BeginScene(m_Camera);
    
    simpleMaterial->SetColor(glm::vec4(1.0f));
    simpleMaterial->SetTextureMap(ground);
    planet->SetMaterial(simpleMaterial);
    planet->DrawModel();
    
    phongMaterial->SetAmbientColor(glm::vec3(0.8f, 0.2f, 0.4f));
    phongMaterial->SetDiffuseColor(glm::vec3(0.8f, 0.2f, 0.4f));
    phongMaterial->SetSpecularColor(glm::vec3(1.0f));
    phongMaterial->SetShininess(100.0f);
    phongMaterial->DefineLightProperties(m_Lights);
    cube->SetMaterial(phongMaterial);
    cube->DrawModel();
    
    m_Lights.Get("Environment")->DrawLight();
    m_Lights.Get("Positional")->DrawLight();
    
    pixc::Renderer::EndScene();

    pixc::RendererCommand::EndRenderPass();
    
    // -------
    pixc::RendererCommand::BeginRenderPass();
    pixc::RendererCommand::SetViewport(0, 0, m_Camera->GetWidth(), m_Camera->GetHeight());
    pixc::RendererCommand::SetClearColor(glm::vec4(0.0f));
    pixc::RendererCommand::Clear();
    
    pixc::Renderer::BeginScene();
    
    simpleMaterial->SetColor(glm::vec4(1.0f));
    simpleMaterial->SetTextureMap(sceneFB->GetColorAttachment(0));
    viewport->SetMaterial(simpleMaterial);
    viewport->DrawModel();
    
    pixc::Renderer::EndScene();

    pixc::RendererCommand::EndRenderPass();
    
    // -------
    
    // Update camera
    m_Camera->OnUpdate(ts);
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
    unsigned int scale = 2;
    m_Framebuffers.Get("Scene")->Resize(scale * e.GetWidth(), scale * e.GetHeight());
    
    // Define the event as handled
    return true;
}
