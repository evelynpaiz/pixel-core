#pragma once

#include "Foundation/Renderer/GraphicsContext.h"

struct GLFWwindow;

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 *  @brief Manages an OpenGL graphics context.
 *
 *  The `OpenGLContext` class is responsible for initializing, managing, and interacting
 *  with an OpenGL rendering context. It uses GLFW to interface with the windowing system.
 */
class OpenGLContext : public GraphicsContext
{
public:
    // Constructor(s)
    // ----------------------------------------
    OpenGLContext(GLFWwindow* windowHandle);
    
    // Initialization
    // ----------------------------------------
    void Init() override;
    
    // Setter(s)
    // ----------------------------------------
    static void SetWindowHints();
    void SetVerticalSync(bool enabled) override;
    
    // Buffers
    // ----------------------------------------
    void SwapBuffers() override;
    
    // OpenGL context variables
    // ----------------------------------------
private:
    ///< Native window (GLFW).
    GLFWwindow* m_WindowHandle;
};

} // namespace pixc
