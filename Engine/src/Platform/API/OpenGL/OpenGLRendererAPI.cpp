#include "enginepch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include "Platform/OpenGL/Buffer/OpenGLBufferUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * Initializes the OpenGL rendering API.
 *
 * This method handles the OpenGL-specific initialization procedures.
 */
void OpenGLRendererAPI::Init()
{}

/**
 * Clear the buffers to preset values.
 *
 * @param buffersActive State of the buffers.
 */
void OpenGLRendererAPI::Clear(const BufferState& buffersActive)
{
    // Clear buffers
    glClear(utils::data::OpenGL::BufferStateToOpenGLMask(buffersActive));
    SetDepthTesting(buffersActive.depthBufferActive);
}

/**
 * Clear the buffers to preset values.
 *
 * @param color Background color.
 * @param buffersActive State of the buffers.
 */
void OpenGLRendererAPI::Clear(const glm::vec4& color, const BufferState& buffersActive)
{
    glClearColor(color.r, color.g, color.b, color.a);
    Clear(buffersActive);
}

/**
 * Render primitives from array data using the specified vertex array.
 *
 * @param drawable The Vertex Array containing the vertex and index buffers for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
 */
void OpenGLRendererAPI::Draw(const std::shared_ptr<Drawable>& drawable, 
                             const PrimitiveType &primitive)
{
    drawable->Bind();
    drawable->GetIndexBuffer()->Bind();
    glDrawElements(utils::OpenGL::PrimitiveTypeToOpenGLType(primitive),
                   drawable->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

/**
 * Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    glViewport(x, y, width, height);
}

/**
 * Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.
 */
void OpenGLRendererAPI::SetDepthTesting(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}
