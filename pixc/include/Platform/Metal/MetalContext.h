#pragma once

#include "Foundation/Renderer/GraphicsContext.h"

struct GLFWwindow;

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 *  @brief Manages a Metal graphics context.
 *
 *  The `MetalContext` class is responsible for initializing, managing, and interacting
 *  with a Metal rendering context. It uses GLFW to interface with the windowing system
 *  and interacts with Metal for rendering on Apple platforms.
 */
class MetalContext : public GraphicsContext
{
public:
    // Constructor(s)
    // ----------------------------------------
    MetalContext(GLFWwindow* windowHandle);
    
    // Initialization
    // ----------------------------------------
    void Init() override;
    
    // Getter(s)
    // ----------------------------------------
    void* GetDevice() const;
    
    void* GetRenderQueue() const;
    void* GetResourceQueue() const;
    
    void* GetCommandBuffer() const;
    void* GetCommandEncoder() const;
    
    void* GetDrawable() const;
    
    // Setter(s)
    // ----------------------------------------
    static void SetWindowHints();
    void SetVerticalSync(bool enabled) override;
    
    // Buffers
    // ----------------------------------------
    void SwapBuffers() override;
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class MetalRendererAPI;
    friend class MetalGuiBackend;
    
private:
    // Initialization
    // ----------------------------------------
    void InitDevice();
    void InitSwapChain();
    
    bool InitCommandBuffer();
    void InitCommandEncoder(const void* descriptor,
                            const std::string &name = "");
    
    // Render
    // ----------------------------------------
    void EndEncoding();
    
    // Metal context variables
    // ----------------------------------------
private:
    ///< Native window (GLFW).
    GLFWwindow* m_WindowHandle;
    
    ///< Holds the core Metal objects required for rendering.
    struct MetalState;
    std::shared_ptr<MetalState> m_State;
};

} // namespace pixc
