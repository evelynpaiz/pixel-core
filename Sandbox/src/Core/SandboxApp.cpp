#include "Core/SandboxApp.h"

#include "Layer/SimpleLayer.h"

/**
 * @brief Generate a (sandbox) rendering application.
 *
 * @param name Name of the application.
 * @param width Size of the window (width).
 * @param height Size of the window (height).
 */
SandboxApp::SandboxApp(const std::string &name, const int width, const int height)
    : Application(name, width, height)
{
    // Define the specific resource path
    pixc::ResourcesManager::SetSpecificPath("/Users/evelynpaiz/Library/CloudStorage/GoogleDrive-evelyn.rpaiz@gmail.com/Mi unidad/Dev/assets");
    
    // Define a rendering and gui layer
    m_Renderer = std::make_shared<SimpleLayer>(GetWindow().GetWidth(), GetWindow().GetHeight());
    m_Gui = std::make_shared<pixc::GuiLayer>();
    
    // Push the layers to the stack
    PushLayer(m_Renderer);
    PushOverlay(m_Gui);
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
