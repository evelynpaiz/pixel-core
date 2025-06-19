#include "enginepch.h"
#include "Foundation/Renderer/RendererCommand.h"

namespace pixc {

std::unique_ptr<RendererAPI> RendererCommand::s_API = RendererAPI::Create();

/**
 * @brief Initialize the renderer command manager by initializing the graphics API.
 */
void RendererCommand::Init()
{
    s_API->Init();
}

/**
 * @brief Define the color to clear the color buffer.
 *
 * @param color The color to use.
 */
void RendererCommand::SetClearColor(const glm::vec4 &color)
{
    s_API->SetClearColor(color);
}

/**
 * @brief Initialize a new rendering pass.
 */
void RendererCommand::BeginRenderPass()
{
    s_API->BeginRenderPass();
}

/**
 * @brief Finalize the current rendering pass.
 */
void RendererCommand::EndRenderPass()
{
   s_API->EndRenderPass();
}

/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param targets Active rendering targets.

void RendererCommand::SetRenderTarget(const RenderTargetBuffers& targets)
{
    s_API->SetRenderTarget(targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param color Background color.
 * @param targets Active rendering targets.

void RendererCommand::SetRenderTarget(const glm::vec4& color,
                                      const RenderTargetBuffers& targets)
{
    s_API->SetRenderTarget(color, targets);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param framebuffer Framebuffer whose targets should be activated.

void RendererCommand::SetRenderTarget(const std::shared_ptr<FrameBuffer>& framebuffer,
                                      const std::optional<RenderTargetBuffers>& targets)
{
    s_API->SetRenderTarget(targets.has_value() ? targets.value() : framebuffer->GetActiveRenderTargets(),
                           framebuffer);
}
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param color Background color.
 * @param framebuffer Framebuffer whose targets should be activated and cleared.

void RendererCommand::SetRenderTarget(const glm::vec4& color,
                                      const std::shared_ptr<FrameBuffer>& framebuffer,
                                      const std::optional<RenderTargetBuffers>& targets)
{
    s_API->SetRenderTarget(color, targets.has_value() ? targets.value() : framebuffer->GetActiveRenderTargets(),
                           framebuffer);
}
 */
/**
 * Render primitives from a drawable object using the specified primitive type.
 *
 * @param drawable The drawable object containing the data for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).

void RendererCommand::Draw(const std::shared_ptr<Drawable>& drawable,
                           const PrimitiveType &primitive)
{
    s_API->Draw(drawable, primitive);
}
 */
/**
 * Finalize the current rendering pass.

void RendererCommand::EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    s_API->EndRenderPass(framebuffer);
}
 */
/**
 * Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.

void RendererCommand::SetViewport(unsigned int x, unsigned int y,
                                  unsigned int width, unsigned int height)
{
    s_API->SetViewport(x, y, width, height);
}
 */
/**
 * Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.

void RendererCommand::SetDepthTesting(const bool enabled)
{
    s_API->SetDepthTesting(enabled);
}
 */
} // namespace pixc
