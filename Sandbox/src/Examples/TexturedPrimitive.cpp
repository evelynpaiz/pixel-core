#include "Examples/TexturedPrimitive.h"

/**
 * @brief Create a rendering layer.
 *
 * @param width Width of the window where the layer is presented.
 * @param height Height of the window where the layer is presented.
 * @param name Name of the layer.
 */
TexturedPrimitive::TexturedPrimitive(const uint32_t width, const uint32_t height,
                                     const std::string& name)
    : RenderingLayer(width, height, name)
{
    // Define the rendering camera
    auto camera = std::make_shared<pixc::OrthographicCamera>(width, height);
    camera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    camera->SetZoomLevel(5.0f);
    m_Scene.SetCamera(camera);
}

/**
 * @brief Define and register materials used for rendering.
 */
void TexturedPrimitive::DefineMaterials()
{
    // Get the material library defined in the renderer
    auto& materialLibrary = pixc::Renderer::GetMaterialLibrary();
    // Define the new material(s)
    materialLibrary.Create<pixc::UnlitMaterial>("Unlit");
}

/**
 * @brief Define the geometry used in the scene.
 */
void TexturedPrimitive::DefineGeometry()
{
    // Define the model(s)
    auto planet = std::make_shared<pixc::AssimpModel>(pixc::ResourcesManager::SpecificPath("models/sample/planet/planet.obj"));
    planet->SetScale(glm::vec3(0.5f));
    m_Scene.GetModels().Add("Planet", planet);
}

/**
 * @brief Defines the rendering passes.
 */
void TexturedPrimitive::DefineRenderPasses()
{
    // Get the rendering passes library defined inside the scene
    auto& library = m_Scene.GetRenderPasses();
    
    // Create a render pass specification for the scene
    pixc::RenderPassSpecification scenePassSpec;
    scenePassSpec.Target.ClearColor = glm::vec4(0.33f, 0.33f, 0.33f, 1.0f);
    scenePassSpec.Target.ClearTargets = pixc::RenderTargetMask::Color | pixc::RenderTargetMask::Depth;
    scenePassSpec.Render.Camera = m_Scene.GetCamera();
    scenePassSpec.Render.Models = {
        { "Planet", "Unlit",
            [](const std::shared_ptr<pixc::Material>& material)
            {
                auto unlitMaterial =  std::dynamic_pointer_cast<pixc::UnlitMaterial>(material);
                
                if (!unlitMaterial)
                    return;

                static auto ground = pixc::Texture2D::CreateFromFile(
                    pixc::ResourcesManager::SpecificPath("models/sample/planet/planet_Quom1200.png"));
                
                unlitMaterial->SetColor(glm::vec4(1.0f));
                unlitMaterial->SetTextureMap(ground);
            }
        }
    };
    library.Add("Scene", scenePassSpec);
}

/**
 * @brief Render the rendering layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void TexturedPrimitive::OnUpdate(pixc::Timestep ts)
{
    // Reset per-frame rendering statistics (e.g., draw calls, triangles, vertices)
    pixc::Renderer::ResetStats();
    // Draw all objects contained in the scene into its assigned viewport
    m_Scene.Draw();
    // Update the camera state (e.g., input handling, matrices) using the timestep
    m_Scene.GetCamera()->OnUpdate(ts);
}
