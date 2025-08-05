#pragma once

#include <pixc.h>

/**
 * @brief Specialized application for rendering engine sandboxing.
 *
 * The `SandboxApp` class derives from `pixc::Application` and provides a setup for testing and
 * showcasing rendering features in a sandbox environment. It initializes the application with a specific window
 * configuration and attaches rendering layers (e.g., `SandboxLayer`) for 3D scene visualization.
 *
 * Copying or moving `SandboxApp` objects is disabled to ensure single ownership and prevent unintended
 * duplication.
 */
class SandboxApp : public pixc::Application
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    SandboxApp(const std::string &name = "3D Viewer Application", const int width = 800,
              const int height = 600);
    ~SandboxApp();
    
    // Viewer application variables
    // ----------------------------------------
private:
    ///< 3D viewer (rendering layer).
    std::shared_ptr<pixc::RenderingLayer> m_Renderer;
    std::shared_ptr<pixc::GuiLayer> m_Gui;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SandboxApp);
};
