#include "Examples/DepthSample.h"

#include <random>

/**
 * @brief Create a rendering layer.
 *
 * @param width Width of the window where the layer is presented.
 * @param height Height of the window where the layer is presented.
 * @param name Name of the layer.
 */
DepthSample::DepthSample(const uint32_t width, const uint32_t height,
                         const std::string& name)
    : RenderingLayer(width, height, name)
{
    // Update the subsampling scale of the viewport
    m_Scene.GetViewport()->ReScale(2.0f);
}

/**
 * @brief Define and register materials used for rendering.
 */
void DepthSample::DefineMaterials()
{
    // Get the material library defined in the renderer
    auto& materialLibrary = pixc::Renderer::GetMaterialLibrary();
    
    // Define the new material(s)
    materialLibrary.Create<pixc::DepthMaterial>("LinearDepth");
}

/**
 * @brief Set up lighting parameters and light sources.
 */
void DepthSample::DefineLights()
{
    uint32_t width = m_Scene.GetCamera()->GetWidth();
    uint32_t height = m_Scene.GetCamera()->GetHeight();
    
    // Define the positional light source(s)
    auto light = std::make_shared<pixc::PositionalLight>(glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    light->InitShadowFrameBuffer(width, height);
    
    m_Scene.GetLights().Add("SingleLight", light);
    
    // Set the depth material properties based on the light information
    auto material = std::dynamic_pointer_cast<pixc::DepthMaterial>(pixc::Renderer::GetMaterialLibrary().Get("LinearDepth"));
    auto& shadowCamera = light->GetShadowCamera();
    shadowCamera->SetFarPlane(20.0f);
    
    material->SetTextureMap(light->GetShadowMap());
    material->SetNearFar(shadowCamera->GetNearPlane(), shadowCamera->GetFarPlane());
}

/**
 * @brief Define the geometry used in the scene.
 */
void DepthSample::DefineGeometry()
{
    auto plane = pixc::utils::geometry::ModelCube<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
    plane->SetScale({20.0f, 1.0f, 20.0f});
    plane->SetPosition({0.0f, -1.0f, 0.0f});
    m_Scene.GetModels().Add("Plane", plane);
    
    constexpr int numCubes = 3;
    for (int i = 0; i < numCubes; ++i)
    {
        auto cube = pixc::utils::geometry::ModelCube<pixc::GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>();
        cube->SetScale(glm::vec3(0.5f));
        m_Scene.GetModels().Add("Cube" + std::to_string(i+1), cube);
    }
}

/**
 * @brief Defines the rendering passes.
 */
void DepthSample::DefineRenderPasses()
{
    // Define a shadow pass from the directional light defined
    auto light = std::dynamic_pointer_cast<pixc::LightCaster>(m_Scene.GetLights().Get("SingleLight"));
    
    pixc::RenderPassSpecification shadowPassSpec;
    shadowPassSpec.Target.FrameBuffer = light->GetShadowFrameBuffer();
    shadowPassSpec.Render.Camera = light->GetShadowCamera();
    shadowPassSpec.Render.Models = {
        { "Plane", "Depth" },
        { "Cube1", "Depth", [](auto& model) { model->SetPosition({-0.6f, 0.0f,  0.5f}); } },
        { "Cube2", "Depth", [](auto& model) { model->SetPosition({ 0.0f, 0.0f,  1.5f}); } },
        { "Cube3", "Depth", [](auto& model) { model->SetPosition({ 1.0f, 0.0f,  3.0f}); } }
    };
    shadowPassSpec.Hooks.PreRenderCode = []() { pixc::RendererCommand::SetFaceCulling(pixc::FaceCulling::Front); };
    shadowPassSpec.Hooks.PostRenderCode = []() { pixc::RendererCommand::SetFaceCulling(pixc::FaceCulling::Back); };
    
    // Add the shadow pass to the library
    auto& library = m_Scene.GetRenderPasses();
    library.Add("ShadowPass", shadowPassSpec);
}

/**
 * @brief Render the rendering layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void DepthSample::OnUpdate(pixc::Timestep ts)
{
    // Reset per-frame rendering statistics (e.g., draw calls, triangles, vertices)
    pixc::Renderer::ResetStats();
    
    // Draw all objects contained in the scene into its assigned viewport
    m_Scene.Draw();
    
    // Render the scene’s viewport content to the screen framebuffer
    auto material = pixc::Renderer::GetMaterialLibrary().Get("LinearDepth");
    m_Scene.GetViewport()->RenderToFrameBuffer(nullptr, material);
}
