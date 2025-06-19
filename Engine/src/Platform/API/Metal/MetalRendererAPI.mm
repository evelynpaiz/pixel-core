#include "enginepch.h"
#include "Platform/Metal/MetalRendererAPI.h"

//#include "Platform/Metal/MetalRendererUtils.h"

//#include "Platform/Metal/Drawable/MetalDrawable.h"
//#include "Platform/Metal/Buffer/MetalIndexBuffer.h"

#include <Metal/Metal.h>
#include <QuartzCore/QuartzCore.h>

namespace pixc {

/**
 * @brief Holds Metal pipeline configuration states.
 */
struct MetalPipelineState
{
    ///< The current render pipeline state.
    id<MTLRenderPipelineState> RenderPipelineState;
    ///< The current depth-stencil state.
    id<MTLDepthStencilState> DepthStencilState;
};

/**
 * @brief Represents the rendering target resources.
 */
struct MetalRenderTarget
{
    ///< The color attachment texture (often from the drawable).
    id<MTLTexture> ColorAttachment;
    ///< The depth attachment texture.
    id<MTLTexture> DepthAttachment;
};

/**
 * @brief Holds the core Metal objects required for rendering within the MetalRendererAPI.
 */
struct MetalRendererAPI::MetalRendererState
{
    ///< Pipeline and depth-stencil state configuration.
    MetalPipelineState PipelineState;
    ///< Render target resources.
    MetalRenderTarget RenderTarget;
    
    ///< Viewport desccriptor.
    MTLViewport Viewport;
    ///< Clear color.
    MTLClearColor ClearColor;
};

/**
 * @brief Initializes the Metal rendering API.
 *
 * This method handles the Metal-specific initialization procedures.
 */
void MetalRendererAPI::Init()
{
    // Get the Metal graphics context and save it
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    PIXEL_CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
    
    // Initialize the Metal state object for managing Metal resources
    m_State = std::make_shared<MetalRendererState>();
    
    // Get the surface to output the render result if rendering to screen
    auto drawable = reinterpret_cast<id<CAMetalDrawable>>(m_Context->GetDrawable());
    m_State->RenderTarget.ColorAttachment = drawable.texture;
}

/**
 * @brief Define the color to clear the color buffer.
 *
 * @param color The color to use.
 */
void MetalRendererAPI::SetClearColor(const glm::vec4& color)
{
    // Set the clear color
    m_State->ClearColor = MTLClearColorMake(color.r, color.g, color.b, color.a);
}

/**
 * Initialize a new rendering pass.
 */
void MetalRendererAPI::BeginRenderPass()
{
    m_Context->InitCommandBuffer();
}

/**
 * Finalize the current rendering pass.
 */
void MetalRendererAPI::EndRenderPass()
{
    m_Context->EndEncoding();
}

/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param targets Active rendering targets.
 
 void MetalRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets)
 {
 SetRenderTarget(glm::vec4(glm::vec4(0.0f)), targets);
 }
 */

/**
 * Sets the active rendering targets and clears the specified buffers.
 *
 * @param color Background color.
 * @param targets Active rendering targets.
 
 void MetalRendererAPI::SetRenderTarget(const glm::vec4& color,
 const RenderTargetBuffers& targets)
 {
 m_Context->SetRenderTarget(color, targets);
 SetDepthTesting(targets.depthBufferActive);
 }
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param framebuffer Framebuffer whose targets should be activated.
 
 void MetalRendererAPI::SetRenderTarget(const RenderTargetBuffers& targets,
 const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 SetRenderTarget(glm::vec4(glm::vec4(0.0f)), targets, framebuffer);
 }
 */
/**
 * Sets the active rendering targets and clears the specified buffers of a framebuffer.
 *
 * @param color Background color.
 * @param framebuffer Framebuffer whose targets should be activated and cleared.
 
 void MetalRendererAPI::SetRenderTarget(const glm::vec4& color,
 const RenderTargetBuffers& targets,
 const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 framebuffer->Bind();
 m_Context->SetRenderTarget(color, targets, framebuffer);
 SetDepthTesting(targets.depthBufferActive);
 }
 */
/**
 * Finalize the current rendering pass.
 
 void MetalRendererAPI::EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
 {
 m_Context->EndEncoding();
 RendererAPI::EndRenderPass(framebuffer);
 }
 */
/**
 * Renders primitives from a drawable object using indexed drawing.
 *
 * @param drawable The drawable object containing the vertex and index buffers for rendering.
 * @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
 
 void MetalRendererAPI::Draw(const std::shared_ptr<Drawable>& drawable,
 const PrimitiveType &primitive)
 {
 // Get the command encoder to encode rendering commands into the buffer
 id<MTLRenderCommandEncoder> encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetEncoder());
 
 // Bind the drawable object
 drawable->Bind();
 // Draw primitives
 auto metalIndexBuffer = std::dynamic_pointer_cast<MetalIndexBuffer>(drawable->GetIndexBuffer());
 CORE_ASSERT(metalIndexBuffer, "Invalid buffer cast - not a Metal index buffer!");
 
 id<MTLBuffer> indexBuffer = reinterpret_cast<id<MTLBuffer>>(metalIndexBuffer->GetBuffer());
 [encoder
 drawIndexedPrimitives:utils::graphics::mtl::ToMetalPrimitive(primitive)
 indexCount:metalIndexBuffer->GetCount()
 indexType:MTLIndexTypeUInt32
 indexBuffer:indexBuffer
 indexBufferOffset:0];
 }
 */
/**
 * Set the viewport for rendering.
 *
 * @param x The x-coordinate of the lower-left corner of the viewport.
 * @param y The y-coordinate of the lower-left corner of the viewport.
 * @param width The width of the viewport.
 * @param height The height of the viewport.
 
 void MetalRendererAPI::SetViewport(unsigned int x, unsigned int y,
 unsigned int width, unsigned int height)
 {
 m_Context->UpdateViewport(x, y, width, height);
 }
 */
/**
 * Set the depth buffer flag when rendering. If enabled, depth testing is enabled too.
 *
 * @param enable Enable or not the depth testing.
 
 void MetalRendererAPI::SetDepthTesting(bool enabled)
 {
 if (!enabled)
 return;
 
 // Defines a depth stencil state into the current command encoder
 m_Context->EnableDepthStencilState();
 }
 */

} // namespace pixc
