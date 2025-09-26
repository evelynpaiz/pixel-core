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
    // Iterate through all render passes in the order they were added
    for (auto& name : m_RenderPasses.m_Order)
    {
        // Retrieve the render pass specification by name
        auto& pass = m_RenderPasses.Get(name);
        if (pass.Active)
        {
            // If the render pass is active, perform the full rendering workflow
            Draw(pass);
        }
        else
        {
            // If the render pass is inactive but has a framebuffer target,
            // apply clear/viewport settings to keep buffers consistent
            if (!pass.Target.FrameBuffer)
                continue;

            RendererCommand::BeginRenderPass(pass.Target.FrameBuffer);
            ApplyTargetSettings(pass.Target);
            RendererCommand::EndRenderPass();
        }
    }
}

/**
 * Draws the scene using the provided render pass specification.
 *
 * @param pass The render pass specification containing the parameters for drawing the scene.
 */
void Scene::Draw(const RenderPassSpecification &pass)
{
    if (!pass.Active)
            return;
    
    // Run pre-render hook
    if (pass.Hooks.PreRenderCode)
        pass.Hooks.PreRenderCode();
    
    // Begin render pass
    RendererCommand::BeginRenderPass(pass.Target.FrameBuffer);
    
    // Apply the settings specific for the rendering target
    ApplyTargetSettings(pass.Target);
    
    // Begin the scene with the specified camera
    Renderer::BeginScene(pass.Render.Camera);
    
    // Render light sources separately
    if (pass.Render.RenderLights)
        DrawLights();
    
    // Render each model
    DrawModels(pass.Render.Models);
    
    // End scene and render pass
    Renderer::EndScene();
    RendererCommand::EndRenderPass();

    // Run post-render hook
    if (pass.Hooks.PostRenderCode)
        pass.Hooks.PostRenderCode();
}

/**
 * Draws the scene lights.
 */
 void Scene::DrawLights()
 {
     for (auto& [name, light] : m_Lights)
         light->DrawLight();
 }

/**
 * @brief Renders a collection of models defined by renderables.
 *
 * @param models The list of renderables to be drawn, including model and material information.
 */
void Scene::DrawModels(const std::vector<Renderable>& models)
{
    for (const auto& renderable : models)
    {
        // Get the model from library
        auto& model = m_Models.Get(renderable.ModelName);
        if (!model)
            continue;

        // Assign material if specified
        if (!renderable.MaterialName.empty())
        {
            auto& material = Renderer::GetMaterialLibrary().Get(renderable.MaterialName);
            if (!material)
                continue;

            if (renderable.MaterialSetupFunction)
                renderable.MaterialSetupFunction(material);

            DefineShadowProperties(material);
            model->SetMaterial(material);
        }

        model->DrawModel();
    }
}

/**
 * @brief Applies target-specific rendering settings for a render pass.
 *
 * @param target The target settings containing framebuffer, viewport, and clear information.
 */
void Scene::ApplyTargetSettings(const TargetSettings& target)
{
    if (target.ViewportSize)
        RendererCommand::SetViewport(0, 0, target.ViewportSize->x, target.ViewportSize->y);

    if (target.ClearEnabled)
    {
        RendererCommand::SetClearColor(target.ClearColor);
        RendererCommand::Clear();
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
