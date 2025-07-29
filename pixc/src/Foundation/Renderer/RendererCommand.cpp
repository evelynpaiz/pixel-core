#include "pixcpch.h"
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
 * @brief Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 */
void RendererCommand::SetViewport(const unsigned int x, const unsigned int y,
                                  const unsigned int width, const unsigned int height)
{
    s_API->SetViewport(x, y, width, height);
}

/**
 * @brief Initialize a new rendering pass.
 *
 * @param framebuffer Buffer to hold to result of the rendered pass.
 */
void RendererCommand::BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    s_API->BeginRenderPass(framebuffer);
}

/**
 * @brief Finalize the current rendering pass.
 */
void RendererCommand::EndRenderPass()
{
   s_API->EndRenderPass();
}

/**
 * @brief Clear the buffers to preset values.
 */
void RendererCommand::Clear()
{
    s_API->Clear();
}

/**
 * @brief Clear the buffers to preset values.
 *
 * @param targets The rendering buffers to be cleared.
 */
void RendererCommand::Clear(const RenderTargetBuffers& targets)
{
    s_API->Clear(targets);
}

/**
 * @brief Render primitives from a drawable object using the specified primitive type.
 *
 * @param drawable The drawable object containing the data for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
 */
void RendererCommand::Draw(const std::shared_ptr<Drawable>& drawable,
                           const PrimitiveType &primitive)
{
    s_API->Draw(drawable, primitive);
}

/**
 * @brief Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.
 * @param function Depth function to be used for depth computation.
 */
void RendererCommand::SetDepthTesting(const bool enabled, const DepthFunction function)
{
    s_API->SetDepthTesting(enabled, function);
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
 * Finalize the current rendering pass.

void RendererCommand::EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
{
    s_API->EndRenderPass(framebuffer);
}
 */
} // namespace pixc
