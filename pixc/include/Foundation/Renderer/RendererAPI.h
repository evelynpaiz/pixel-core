#pragma once

#include "Foundation/Renderer/RendererTypes.h"

#include "Foundation/Renderer/Drawable/Drawable.h"
#include "Foundation/Renderer/Buffer/FrameBuffer.h"

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
    
    // Setter(s)
    // ----------------------------------------
    virtual void SetClearColor(const glm::vec4& color) = 0;
    
    virtual void SetViewport(const uint32_t x, const uint32_t y,
                             const uint32_t width, const uint32_t height) = 0;
    
    virtual void EnableDepthTesting(const bool enabled) = 0;
    virtual void SetDepthFunction(const DepthFunction function) = 0;
    /// @brief Configure the full depth test state in one call.
    /// @param enabled Pass true to enable depth testing, false to disable it.
    /// @param function The depth function to use for depth comparisons.
    virtual void ConfigureDepthTesting(const bool enabled, const DepthFunction function)
    {
        EnableDepthTesting(enabled);
        SetDepthFunction(function);
    }
    
    virtual void SetFaceCulling(const FaceCulling mode) = 0;
    virtual void SetCubeMapSeamless(const bool enabled) = 0;
    
    // Render
    // ----------------------------------------
    virtual void BeginRenderPass(const std::shared_ptr<FrameBuffer>& framebuffer);
    virtual void EndRenderPass();
    
    void Clear();
    virtual void Clear(const RenderTargetMask targets) = 0;
    
    virtual void Draw(const std::shared_ptr<Drawable>& drawable,
                      const PrimitiveType &primitive = PrimitiveType::Triangle) = 0;
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Protected default constructor to prevent direct instantiation.
    RendererAPI() = default;
    
    // Renderer API variables
    // ----------------------------------------
protected:
    ///< The currently bound framebuffer. If null, rendering goes to the screen (default framebuffer).
    std::shared_ptr<FrameBuffer> m_ActiveFrameBuffer = nullptr;
    
    
private:
    ///< The currently active rendering API.
    static API s_API;
};

} // namespace pixc
