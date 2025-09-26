#include "pixcpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Platform/OpenGL/OpenGLRendererUtils.h"

namespace pixc {

bool OpenGLRendererAPI::s_IsRendering = false;

/**
 * @brief Initializes the OpenGL rendering API.
 *
 * This method handles the OpenGL-specific initialization procedures.
 */
void OpenGLRendererAPI::Init()
{}

/**
 * @brief Define the color to clear the color buffer.
 *
 * @param color The color to use.
 */
void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

/**
 * @brief Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void OpenGLRendererAPI::SetViewport(const uint32_t x, const uint32_t y,
                                    const uint32_t width, const uint32_t height)
{
   glViewport(x, y, width, height);
}

/**
 * @brief Enable or disable depth testing.
 *
 * @param enabled Pass true to enable depth testing, false to disable it.
 */
void OpenGLRendererAPI::EnableDepthTesting(const bool enabled)
{
    // Define depth test
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

/**
 * @brief Set the depth comparison function used during depth testing.
 *
 * @param function The depth function to use (e.g., Less, LessEqual, Greater, Always).
 */
void OpenGLRendererAPI::SetDepthFunction(const DepthFunction function)
{
    if (function == DepthFunction::None)
        return;
    
    // Change the depth function
    glDepthFunc(utils::graphics::gl::ToOpenGLDepthFunc(function));
}

/**
 * @brief Set the face culling mode for rendering.
 *
 * Face culling is a technique used to improve rendering performance by discarding
 * the rendering of faces that are not visible, such as the back faces of 3D objects.
 *
 * @param mode The face culling mode to be set.
 */
void OpenGLRendererAPI::SetFaceCulling(const FaceCulling mode)
{
    glCullFace(utils::graphics::gl::ToOpenGLCulling(mode));
}

/**
 * @brief Enable or disable seamless cubemap sampling.
 *
 * This function allows you to enable or disable seamless cubemap sampling, which
 * can improve the visual quality when rendering cubemaps, especially when used as
 * skyboxes or for environment mapping.
 *
 * @param enabled Set to `true` to enable seamless cubemap sampling, or `false` to disable it.
 */
void OpenGLRendererAPI::SetCubeMapSeamless(const bool enabled)
{
    if (enabled)
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    else
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

/**
 * @brief Initialize a new rendering pass.
 *
 * @param framebuffer Buffer to hold to result of the rendered pass.
 */
void OpenGLRendererAPI::BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    // Reset the flag defining if something is currently being rendering
    if (!s_IsRendering)
        s_IsRendering = true;
    
    // Update the information of the currently bound framebuffer
    RendererAPI::BeginRenderPass(framebuffer);
    
    if (!m_ActiveFrameBuffer)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);       // render to screen
}

/**
 * @brief Clear the buffers to preset values.
 *
 * @param targets The rendering buffers to be cleared.
 */
void OpenGLRendererAPI::Clear(const RenderTargetBuffers& targets)
{
    // Set depth testing state so it's in the correct state
    EnableDepthTesting(targets.Depth);
    // Clear buffers
    glClear(utils::graphics::gl::ToOpenGLClearMask(targets));
}

/**
 * @brief Render primitives from array data using the specified vertex array.
 *
 * @param drawable The Vertex Array containing the vertex and index buffers for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
 */
void OpenGLRendererAPI::Draw(const std::shared_ptr<Drawable>& drawable,
                             const PrimitiveType &primitive)
{
    drawable->Bind();
    glDrawElements(utils::graphics::gl::ToOpenGLPrimitive(primitive),
                   drawable->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    drawable->Unbind();
}

} // namespace pixc
