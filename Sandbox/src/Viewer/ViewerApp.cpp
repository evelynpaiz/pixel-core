#include "Viewer/ViewerApp.h"

/**
 * @brief Generate a (sandbox) rendering application.
 *
 * @param name Name of the application.
 * @param width Size of the window (width).
 * @param height Size of the window (height).
 */
ViewerApp::ViewerApp(const std::string &name, const int width, const int height)
    : Application(name, width, height)
{
    // Push the viewer layer to the layer stack
    //m_Viewer = std::make_shared<Simple>(GetWindow().GetWidth(), GetWindow().GetHeight());
    m_Gui = std::make_shared<pixc::GuiLayer>();
    
    // TODO: Add support of imgui with metal
    //PushLayer(m_Viewer);
    PushOverlay(m_Gui);
}

/**
 * @brief Delete this application.
 */
ViewerApp::~ViewerApp()
{
    // TODO: Add support of imgui with metal
    //PopLayer(m_Viewer);
    PopOverlay(m_Gui);
}
