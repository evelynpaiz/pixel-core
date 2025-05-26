#pragma once

#include "Common/Renderer/GraphicsContext.h"

#include "Common/Renderer/Buffer/Buffer.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

struct GLFWwindow;

struct MetalRenderState;

/**
 *  Manages a Metal graphics context.
 *
 *  The `MetalContext` class is responsible for initializing, managing, and interacting
 *  with a Metal rendering context. It uses GLFW to interface with the windowing system
 *   and interacts with Metal for rendering on Apple platforms.
 */
class MetalContext : public GraphicsContext
{
public:
    // Constructor(s)
    // ----------------------------------------
    MetalContext(GLFWwindow* windowHandle);

    // Initialization
    // ----------------------------------------
    virtual void Init() override;
    
    // Getter(s)
    // ----------------------------------------
    void* GetDevice() const;
    
    void* GetRenderQueue() const;
    void* GetResourceQueue() const;
    
    void* GetCommandBuffer() const;
    void* GetEncoder() const;
    
    // Setter(s)
    // ----------------------------------------
    static void SetWindowHints();
    void SetVerticalSync(bool enabled) override;
    
    void UpdateScreenbufferSize(unsigned int width, unsigned int height) override;
    void UpdateViewport(unsigned int x, unsigned int y,
                        unsigned int width, unsigned int height);
    
    void EnableDepthStencilState();
    
    // Render
    // ----------------------------------------
    void SetRenderTarget(const glm::vec4& color,
                         const RenderTargetBuffers& targets,
                         const std::shared_ptr<FrameBuffer>& framebuffer = nullptr);
    void EndEncoding();
    
    // Draw
    // ----------------------------------------
    void SwapBuffers() override;
    
private:
    // Initialization
    // ----------------------------------------
    void InitializeMetalDeviceResources();
    void InitializeScreenTarget();
    
    void CreateScreenDepthTexture();
    
    void CreateDepthStencilState();
    
    // Graphics context variables
    // ----------------------------------------
private:
    ///< Native window (GLFW).
    GLFWwindow* m_WindowHandle;
    
    ///< Holds the core Metal objects required for rendering.
    struct MetalState;
    std::shared_ptr<MetalState> m_State;
};
