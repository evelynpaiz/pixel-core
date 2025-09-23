#pragma once

#include <glm/glm.hpp>

#include "Foundation/Renderer/GraphicsContext.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc::utils::cubemap {

/**
 * @brief Represents a cubemap with projection and view matrices for all six faces.
 *
 * The `CubeMap` struct stores the necessary data for rendering a cubemap, including:
 *  - A perspective projection matrix used when rendering each face of the cubemap.
 *  - An array of six view matrices corresponding to the positive and negative X, Y, and Z faces.
 */
struct CubeMap
{
    glm::mat4 projection;               ///< Perspective projection matrix for the cubemap.
    std::array<glm::mat4, 6> views;     ///< View matrices for each face of the cubemap (POSX, NEGX, POSY, NEGY, POSZ, NEGZ).
};

/**
 * @brief Build the view matrices for each face of a cubemap.
 *
 * @param api The rendering API to select the correct Y-axis orientation.
 * @param near Near plane distance.
 * @param far Far plane distance.
 * @param fov The field of view in degrees for the projection matrix.
 * @param eye The origin of the cubemap (usually 0,0,0).
 *
 * @return a cube map struct containing projection and view matrices.
 */
inline CubeMap BuildCubeMap(RendererAPI::API api, float near = 0.1f,
                            float far = 10.0f, float fov = 90.0f,
                            const glm::vec3& eye = glm::vec3(0.0f))
{
    // Construct the perspective projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(fov), 1.0f, near, far);

    // Define view matrices for each face of the cubemap
    const glm::mat4 POSITIVE_X = glm::lookAt(eye, eye + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0));
    const glm::mat4 NEGATIVE_X = glm::lookAt(eye, eye + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0));
    const glm::mat4 POSITIVE_Y = glm::lookAt(eye, eye + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1));
    const glm::mat4 NEGATIVE_Y = glm::lookAt(eye, eye + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1));
    const glm::mat4 POSITIVE_Z = glm::lookAt(eye, eye + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0));
    const glm::mat4 NEGATIVE_Z = glm::lookAt(eye, eye + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0));

    std::array<glm::mat4, 6> views;
    if(api == RendererAPI::API::Metal)
        views = { POSITIVE_X, NEGATIVE_X, NEGATIVE_Y, POSITIVE_Y, POSITIVE_Z, NEGATIVE_Z };
    else
        views = { POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z };

    // Return a struct containing both projection and view matrices
    return { proj, views };
}

/**
 * @brief Apply a rotation to all faces of a CubeMap.
 *
 * @param cube The CubeMap to rotate.
 * @param rotationMatrix The rotation to apply.
 */
inline void RotateCubeMap(CubeMap& cube, const glm::mat4& rotationMatrix)
{
    for(auto& view : cube.views)
        view = view * rotationMatrix;
}

/**
 * Render a cube map from multiple perspectives using the specified material and framebuffer.
 *
 * @param cube The CubeMap containing the projection and view matrices for all six faces.
 * @param model The model to render for each face of the cubemap.
 * @param material The material to use when rendering the model.
 * @param framebuffer The framebuffer where the cubemap faces will be rendered.
 * @param viewportWidth Optional width of the viewport. If zero, the framebuffer default size is used.
 * @param viewportHeight Optional height of the viewport. If zero, the framebuffer default size is used.
 * @param level The mip level of the framebuffer to render into.
 * @param genMipMaps Flag indicating whether to generate mipmaps for the resulting cubemap.
 */
inline void RenderCubeMap(const CubeMap& cube, const std::shared_ptr<BaseModel>& model,
                          const std::shared_ptr<Material>& material,
                          const std::shared_ptr<FrameBuffer>& framebuffer,
                          const uint32_t& viewportWidth = 0,
                          const uint32_t& viewportHeight = 0,
                          const uint32_t& level = 0,
                          const bool& genMipMaps = true)
{
    // Update the size of the model
    model->SetScale(glm::vec3(2.0f));
    // Set the material for rendering
    model->SetMaterial(material);
    
    // Loop through each face of the cube map
    for (uint32_t i = 0; i < cube.views.size(); ++i)
    {
        // Bind the framebuffer for drawing to the current cube map face and mip level
        framebuffer->BindForDrawAttachmentCube(0, i, level);
        
        // Set the viewport dimensions if provided
        if (viewportWidth > 0 && viewportHeight > 0)
            RendererCommand::SetViewport(0, 0, viewportWidth, viewportHeight);
        
        // Clear the active buffers in the framebuffer
        RendererCommand::BeginRenderPass(framebuffer);
        RendererCommand::SetClearColor(glm::vec4(0.0f));
        RendererCommand::Clear();
        
        // Begin rendering scene with the specified view and projection matrices
        Renderer::BeginScene(cube.views[i], cube.projection);
        // Draw the model using the specified material
        model->DrawModel();
        // End the rendering scene
        Renderer::EndScene();
        
        RendererCommand::EndRenderPass();
        
        // Unbind the framebuffer and optionally generate mipmaps
        framebuffer->Unbind(genMipMaps);
        
        // Render the face of the cubemap
        GraphicsContext::Get().SwapBuffers();
    }
}

} // namespace pixc
