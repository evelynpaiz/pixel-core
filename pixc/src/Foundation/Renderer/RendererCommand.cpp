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
void RendererCommand::SetViewport(const uint32_t x, const uint32_t y,
                                  const uint32_t width, const uint32_t height)
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
 * @param targets Bitmask of render targets to be cleared.
 */
void RendererCommand::Clear(const RenderTargetMask targets)
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
 * @brief Enable or disable depth testing.
 *
 * @param enabled Pass true to enable depth testing, false to disable it.
 */
void RendererCommand::EnableDepthTesting(const bool enabled)
{
    s_API->EnableDepthTesting(enabled);
}

/**
 * @brief Set the depth comparison function used during depth testing.
 *
 * @param function The depth function to use (e.g., Less, LessEqual, Greater, Always).
 */
void RendererCommand::SetDepthFunction(const DepthFunction function)
{
    s_API->SetDepthFunction(function);
}

/**
 * @brief Configure the full depth test state in one call.
 *
 * @param enabled Pass true to enable depth testing, false to disable it.
 * @param function The depth function to use for depth comparisons.
 */
void RendererCommand::ConfigureDepthTesting(const bool enabled,
                                            const DepthFunction function)
{
    s_API->ConfigureDepthTesting(enabled, function);
}

/**
 * @brief Set the face culling mode for rendering.
 *
 * @param mode The face culling mode to be set.
 */
void RendererCommand::SetFaceCulling(const FaceCulling mode)
{
    s_API->SetFaceCulling(mode);
}

/**
 * @brief Enable or disable seamless cubemap sampling.
 *
 * @param enabled Set to `true` to enable seamless cubemap sampling, or `false` to disable it.
 */
void RendererCommand::SetCubeMapSeamless(const bool enabled)
{
    s_API->SetCubeMapSeamless(enabled);
}

} // namespace pixc
