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
void OpenGLRendererAPI::SetViewport(const unsigned int x, const unsigned int y,
                                    const unsigned int width, const unsigned int height)
{
   glViewport(x, y, width, height);
}

/**
 * @brief Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.
 * @param function Depth function to be used for depth computation.
 */
void OpenGLRendererAPI::SetDepthTesting(const bool enabled,
                                        const DepthFunction function)
{
    // Define depth test
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    // Define depth function
    if (function != DepthFunction::None)
    {
        glDepthFunc(utils::graphics::gl::ToOpenGLDepthFunc(function));
    }
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
    
    if (!m_ActiveFramebuffer)
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
    SetDepthTesting(targets.depthBufferActive, DepthFunction::None);
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
