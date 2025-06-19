#pragma once

//#include "Foundation/Renderer/Drawable/Drawable.h"
//#include "Foundation/Renderer/Material/Material.h"
//#include "Foundation/Renderer/Buffer/FrameBuffer.h"

//#include "Foundation/Renderer/RendererUtils.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Abstract base class for rendering APIs.
 *
 * The `RendererAPI` class defines a common interface for different rendering APIs,
 * such as OpenGL, Metal, etc.. It cannot be instantiated directly but serves as a base for
 * concrete rendering API implementations.
 */
class RendererAPI
{
public:
    /**
     * Enumeration of supported rendering APIs.
     */
    enum class API
    {
        None = 0,
        OpenGL = 1,
        
#ifdef __APPLE__
        Metal = 2,
#endif
    };
    
public:
    // Destructor
    // ----------------------------------------
    /// @brief Virtual destructor for proper cleanup.
    virtual ~RendererAPI() = default;
    
    // Initialization
    // ----------------------------------------
    /// @brief Pure virtual function for initializing the rendering API.
    virtual void Init() = 0;
    static std::unique_ptr<RendererAPI> Create();
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves the currently active rendering API.
    /// @return The active rendering API.
    static API GetAPI() { return s_API; }
    
    // Render
    // ----------------------------------------
    virtual void SetClearColor(const glm::vec4& color) = 0;
    
    // TODO: Define the rendering pass methods with a framebuffer.
    virtual void BeginRenderPass() = 0;
    virtual void EndRenderPass() = 0;
    
    // Clear
    // ----------------------------------------
    /*
     virtual void SetRenderTarget(const RenderTargetBuffers& targets) = 0;
     virtual void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets) = 0;
     
     virtual void SetRenderTarget(const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) = 0;
     virtual void SetRenderTarget(const glm::vec4& color,
     const RenderTargetBuffers& targets,
     const std::shared_ptr<FrameBuffer>& framebuffer) = 0;
     
     /// @brief Finish rendering pass by unbiding the framebuffer if existent.
     /// @param framebuffer The framebuffer where it was rendered into.
     virtual void EndRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer)
     {
     if (framebuffer)
     framebuffer->Unbind();
     }
     
     // Draw
     // ----------------------------------------
     virtual void Draw(const std::shared_ptr<Drawable>& drawable,
     const PrimitiveType &primitive = PrimitiveType::Triangle) = 0;
     
     // Setter(s)
     // ----------------------------------------
     virtual void SetViewport(unsigned int x, unsigned int y,
     unsigned int width, unsigned int height) = 0;
     
     virtual void SetDepthTesting(const bool enabled) = 0;
     */
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Protected default constructor to prevent direct instantiation.
    RendererAPI() = default;
    
    // Renderer API variables
    // ----------------------------------------
private:
    ///< The currently active rendering API.
    static API s_API;
};

} // namespace pixc
