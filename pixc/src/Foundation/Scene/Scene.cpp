#include "pixc.h"
#include "Foundation/Scene/Scene.h"

namespace pixc {

/**
 * Define a scene to be rendered.
 *
 * @param width Viewport width.
 * @param height Viewport height.
 * @param viewportShader The shader file path to be used for shading the viewport.
 */
Scene::Scene(uint32_t width, uint32_t height,
             const std::filesystem::path& viewportShader)
{
    // Define a generic rendering camera (can be changed with the method SetCamera())
    m_Camera = std::make_shared<pixc::PerspectiveCamera>(width, height);
    
    // Define the viewport
    m_Viewport = std::make_shared<Viewport>(width, height, viewportShader);
    // Add viewport to the model and framebuffer library
    m_Models.Add("Viewport", m_Viewport->s_Geometry);
    m_FrameBuffers.Add("ScreenBuffer", m_Viewport->m_ScreenBuffer);
}

/**
 * Draws the scene according to the specified render passes.
 */
void Scene::Draw()
{
    for (auto& name : m_RenderPasses.m_Order)
    {
        auto& pass = m_RenderPasses.Get(name);
        if (pass.Active)
            Draw(pass);
        
        // TODO: handle not active passes
        /*
         else
         {
         if (pass.Framebuffer)
            pass.Framebuffer->Bind();
         RendererCommand::SetRenderTarget(glm::vec4(0.0f));
         }
         */
    }
}

/**
 * Draws the scene using the provided render pass specification.
 *
 * @param pass The render pass specification containing the parameters for drawing the scene.
 */
void Scene::Draw(const RenderPassSpecification &pass)
{
    // Run pre-render code
    if (pass.PreRenderCode)
        pass.PreRenderCode();
    
    // Begin render pass
    RendererCommand::BeginRenderPass(pass.FrameBuffer);
    
    // Set viewport if specified
    if (pass.ViewportSize)
        RendererCommand::SetViewport(0, 0, (*pass.ViewportSize).x, (*pass.ViewportSize).y);
    
    // Clear framebuffer if enabled
    switch (pass.ClearBehavior)
    {
        case RenderPassSpecification::ClearMode::Enabled:
            // Clear with the specified color
            RendererCommand::SetClearColor(pass.ClearColor);
            RendererCommand::Clear();
            break;

        case RenderPassSpecification::ClearMode::Disabled:
            // Do nothing; leave framebuffer as-is
            break;

        case RenderPassSpecification::ClearMode::Default:
            // Optional: define default behavior (e.g., clear with black)
            RendererCommand::SetClearColor(glm::vec4(0.0f));
            RendererCommand::Clear();
            break;
    }
    
    // Begin the scene with the specified camera
    Renderer::BeginScene(pass.Camera);
    
    // Render light sources separately
    if (pass.RenderLights)
        DrawLights();
    
    // Render each model
    for (const auto& renderable : pass.Models)
    {
        
        auto& model = m_Models.Get(renderable.ModelName);
        if (!model)
            continue;

        if (!renderable.MaterialName.empty())
        {
            auto& material = Renderer::GetMaterialLibrary().Get(renderable.MaterialName);
            if (renderable.MaterialSetupFunction)
                renderable.MaterialSetupFunction(material);
            DefineShadowProperties(material);
            model->SetMaterial(material);
        }

        model->DrawModel();
    }
    
    // End scene and render pass
    Renderer::EndScene();
    RendererCommand::EndRenderPass();

    // Run post-render code
    if (pass.PostRenderCode)
        pass.PostRenderCode();
}

/**
 * Draws the scene lights.
 */
 void Scene::DrawLights()
 {
     for (auto& pair : m_Lights)
     {
         auto& light = pair.second;
         light->DrawLight();
     }
 }

/**
 * Define shadow properties for a given material.
 *
 * @param baseMaterial The base material to define shadow properties for.
 * @note If the "Shadow" render pass does not exist in the scene, no shadows maps will be defined.
 */
 void Scene::DefineShadowProperties(const std::shared_ptr<Material>& baseMaterial)
 {
     // Attempt to cast the base material to a LightedMaterial
     auto material = std::dynamic_pointer_cast<LightedMaterial>(baseMaterial);
     if (!material)
         return; // Base material is not a LightedMaterial, so return early
     
     // Define the properties of the material related to the light sources
     material->DefineLightProperties(m_Lights);
 }

} // namespace pixc
