#pragma once

#include "Common/Renderer/Drawable/Drawable.h"
#include "Common/Renderer/Material/Material.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/RendererUtils.h"

#include <glm/glm.hpp>

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
    
    // Clear
    // ----------------------------------------
    virtual void SetRenderTarget(const RenderTargetBuffers& targets) = 0;
    virtual void SetRenderTarget(const glm::vec4& color,
                                 const RenderTargetBuffers& targets) = 0;
    
    virtual void SetRenderTarget(const RenderTargetBuffers& targets,
                                 const std::shared_ptr<FrameBuffer>& framebuffer) = 0;
    virtual void SetRenderTarget(const glm::vec4& color,
                                 const RenderTargetBuffers& targets,
                                 const std::shared_ptr<FrameBuffer>& framebuffer) = 0;
    
    virtual void EndRenderPass() = 0;
    
    // Draw
    // ----------------------------------------
    virtual void Draw(const std::shared_ptr<Drawable>& drawable,
                      const PrimitiveType &primitive = PrimitiveType::Triangle) = 0;
    
    // Setter(s)
    // ----------------------------------------
    virtual void SetViewport(unsigned int x, unsigned int y,
                            unsigned int width, unsigned int height) = 0;
    
    virtual void SetDepthTesting(const bool enabled) = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves the currently active rendering API.
    /// @return The active rendering API.
    static API GetAPI() { return s_API; }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Protected default constructor to prevent direct instantiation.
    RendererAPI() = default;
    
private:
    // Graphics context
    // ----------------------------------------
    bool IsGraphicsContextActive();
    
    // Renderer API variables
    // ----------------------------------------
private:
    ///< The currently active rendering API.
    static API s_API;
};

#ifdef __APPLE__
    #define CREATE_RENDERER_OBJECT(ObjectType, ...)\
        switch (Renderer::GetAPI())\
        {\
            case RendererAPI::API::None:\
                CORE_ASSERT(false, "RendererAPI::None is not supported!");\
                return nullptr;\
            case RendererAPI::API::OpenGL:\
                return std::make_shared<OpenGL##ObjectType>(__VA_ARGS__);\
            case RendererAPI::API::Metal:\
                return std::make_shared<Metal##ObjectType>(__VA_ARGS__);\
        }\
        CORE_ASSERT(false, "Unknown Renderer API!");\
        return nullptr;
#else
    #define CREATE_RENDERER_OBJECT(ObjectType, ...)\
        switch (Renderer::GetAPI())\
        {\
            case RendererAPI::API::None:\
                CORE_ASSERT(false, "RendererAPI::None is not supported!");\
                return nullptr;\
            case RendererAPI::API::OpenGL:\
                return std::make_shared<OpenGL##ObjectType>(__VA_ARGS__);\
        }\
        CORE_ASSERT(false, "Unknown Renderer API!");\
        return nullptr;
#endif
