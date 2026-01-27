#include "App/SandboxApp.h"

#include "Examples/BasicSample.h"
#include "Examples/UnlitSample.h"
#include "Examples/DepthSample.h"
#include "Examples/PhongSample.h"

/**
 * @brief Generate a (sandbox) rendering application.
 *
 * @param name Name of the application.
 * @param width Size of the window (width).
 * @param height Size of the window (height).
 */
SandboxApp::SandboxApp(const std::string &name, const uint32_t width, const uint32_t height)
    : Application(name, width, height)
{
    // Define the specific resource path
    pixc::ResourcesManager::SetSpecificPath("/Users/evelynpaiz/Library/CloudStorage/GoogleDrive-evelyn.rpaiz@gmail.com/Mi unidad/Dev/assets");
    
    // Define a rendering and gui layer
    m_Renderer = InitRenderingLayer(SandboxLayerType::Phong, width, height);
    m_Gui = std::make_shared<pixc::GuiLayer>();
    
    // Push the layers to the stack
    PushLayer(m_Renderer);
    PushOverlay(m_Gui);
}

/**
 * @brief Create a rendering layer based on the selected type.
 *
 * @param type The type of layer to create.
 * @param width Width of the viewport/window.
 * @param height Height of the viewport/window.
 * @return A shared pointer to the created rendering layer.
 */
std::shared_ptr<pixc::Layer> SandboxApp::InitRenderingLayer(SandboxLayerType type,
                                                            uint32_t width, uint32_t height)
{
    switch (type)
    {
        case SandboxLayerType::Basic:
            return std::make_shared<BasicSample>(width, height);
        case SandboxLayerType::Unlit:
            return std::make_shared<UnlitSample>(width, height);
        case SandboxLayerType::Depth:
            return std::make_shared<DepthSample>(width, height);
        case SandboxLayerType::Phong:
            return std::make_shared<PhongSample>(width, height);
    }
    
    return nullptr;
}

/**
 * @brief Delete this application.
 */
SandboxApp::~SandboxApp()
{
    // Pop the layers from the stack before closing the application
    PopLayer(m_Renderer);
    PopOverlay(m_Gui);
}
