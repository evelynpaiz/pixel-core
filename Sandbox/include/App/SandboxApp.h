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
    SandboxApp(const std::string &name = "Pixel-Core Sandbox Application",
               const uint32_t width = 800, const uint32_t height = 600);
    ~SandboxApp();
    
private:
    /**
     * @brief Enum for selecting which rendering layer to use in the sandbox.
     */
    enum class SandboxLayerType
    {
        Basic,      ///< Use BasicPrimitive layer
        Textured,   ///< Use TexturedPrimitive layer
        Phong       ///< Use PhongPrimitive layer
    };
    std::shared_ptr<pixc::Layer> InitRenderingLayer(SandboxLayerType type,
                                                    uint32_t width, uint32_t height);
    
    // Viewer application variables
    // ----------------------------------------
private:
    ///< 3D viewer (rendering layer).
    std::shared_ptr<pixc::Layer> m_Renderer;
    std::shared_ptr<pixc::Layer> m_Gui;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(SandboxApp);
};
