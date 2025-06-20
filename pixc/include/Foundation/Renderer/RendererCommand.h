#pragma once

#include "Foundation/Renderer/RendererAPI.h"

#include <glm/glm.hpp>
#include <optional>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * A static class that provides an interface to the rendering API.
 *
 * The `RendererCommand` class provides a set of static methods that can be used
 * to interact with the rendering API. It uses a singleton pattern to ensure that only one
 * instance of the Renderer API is created.
 */
class RendererCommand
{
public:
    // Initialization
    // ----------------------------------------
    static void Init();
    
    // Setter(s)
    // ----------------------------------------
    static void SetClearColor(const glm::vec4& color);
    
    static void SetViewport(const unsigned int x, const unsigned int y,
                            const unsigned int width, const unsigned int height);
    
    // Render
    // ----------------------------------------
    static void BeginRenderPass();
    static void EndRenderPass();
    
    static void Clear();
    
    /*
     static void SetRenderTarget(const RenderTargetBuffers& targets = {});
     static void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets = {});
     
     static void SetRenderTarget(const std::shared_ptr<FrameBuffer>& framebuffer,
     const std::optional<RenderTargetBuffers>& targets = std::nullopt);
     static void SetRenderTarget(const glm::vec4& color,
     const std::shared_ptr<FrameBuffer>& framebuffer,
     const std::optional<RenderTargetBuffers>& targets = std::nullopt);
     
     static void EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer = nullptr);
     */
    // Draw
    // ----------------------------------------
    //static void Draw(const std::shared_ptr<Drawable>& drawable,
    //                 const PrimitiveType &primitive = PrimitiveType::Triangle);
    
    // Setter(s)
    // ----------------------------------------
    
    //static void SetDepthTesting(const bool enabled);
    
    // Renderer variables
    // ----------------------------------------
private:
    ///< Rendering API.
    static std::unique_ptr<RendererAPI> s_API;
};

} // namespace pixc
