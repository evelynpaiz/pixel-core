#include "enginepch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "Platform/OpenGL/OpenGLRendererUtils.h"

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
 * Initialize a new rendering pass.
 */
void OpenGLRendererAPI::BeginRenderPass()
{
    if (!s_IsRendering)
        s_IsRendering = true;
}

/**
 * Finalize the current rendering pass.
 */
void OpenGLRendererAPI::EndRenderPass()
{}

/**
 * Clears the specified rendering buffers.
 *
 * @param targets The rendering buffers to be cleared.

void OpenGLRendererAPI::Clear(const RenderTargetBuffers &targets)
{
    glClear(utils::graphics::gl::ToOpenGLClearMask(targets));
    SetDepthTesting(targets.depthBufferActive);
}
 */
/**
 * Sets the background color and clears the specified rendering buffers.
 *
 * @param color The RGBA color used to clear the color buffer.
 * @param targets The rendering buffers to be cleared.

void OpenGLRendererAPI::Clear(const glm::vec4& color,
                              const RenderTargetBuffers& targets)
{
    glClearColor(color.r, color.g, color.b, color.a);
    Clear(targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param targets Active rendering targets.

void OpenGLRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Clear(targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param color Background color.
 * @param targets Active rendering targets.

void OpenGLRendererAPI::SetRenderTarget(const glm::vec4& color,
                                        const RenderTargetBuffers& targets)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Clear(color, targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param framebuffer Framebuffer whose targets should be activated.

void OpenGLRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets,
                                        const std::shared_ptr<FrameBuffer>& framebuffer)
{
    framebuffer->Bind();
    Clear(targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param color Background color.
 * @param framebuffer Framebuffer whose targets should be activated and cleared.

void OpenGLRendererAPI::SetRenderTarget(const glm::vec4& color,
                                        const RenderTargetBuffers& targets,
                                        const std::shared_ptr<FrameBuffer>& framebuffer)
{
    framebuffer->Bind();
    Clear(color, targets);
}
 */
/**
 * Render primitives from array data using the specified vertex array.
 *
 * @param drawable The Vertex Array containing the vertex and index buffers for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).

void OpenGLRendererAPI::Draw(const std::shared_ptr<Drawable>& drawable,
                             const PrimitiveType &primitive)
{
    drawable->Bind();
    drawable->GetIndexBuffer()->Bind();
    glDrawElements(utils::graphics::gl::ToOpenGLPrimitive(primitive),
                   drawable->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
 */
/**
 * Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.

void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y,
                                    unsigned int width, unsigned int height)
{
    glViewport(x, y, width, height);
}
 */
/**
 * Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.

void OpenGLRendererAPI::SetDepthTesting(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
 */
} // namespace pixc
