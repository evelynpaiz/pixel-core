#pragma once

#include "Foundation/Renderer/RendererAPI.h"

#include "Foundation/Renderer/Buffer/FrameBuffer.h"

#include <glm/glm.hpp>

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
    
    static void SetViewport(const uint32_t x, const uint32_t y,
                            const uint32_t width, const uint32_t height);
    
    // Render
    // ----------------------------------------
    static void BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer = nullptr);
    static void EndRenderPass();
    
    static void Clear();
    static void Clear(const RenderTargetBuffers& targets);
    
    static void Draw(const std::shared_ptr<Drawable>& drawable,
                     const PrimitiveType &primitive = PrimitiveType::Triangle);
    
    // Setter(s)
    // ----------------------------------------
    static void EnableDepthTesting(const bool enabled = true);
    static void SetDepthFunction(const DepthFunction function);
    static void ConfigureDepthTesting(const bool enabled = true,
                                      const DepthFunction function = DepthFunction::Less);
    
    // Renderer variables
    // ----------------------------------------
private:
    ///< Rendering API.
    static std::unique_ptr<RendererAPI> s_API;
};

} // namespace pixc
