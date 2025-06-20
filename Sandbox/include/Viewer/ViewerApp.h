#pragma once

#include <pixc.h>

/**
 * @brief Handles a 3D viewer application.
 *
 * The `ViewerApp` class is a derived class of the `Application` class, specifically designed for creating
 * a 3D viewer application. It inherits all the properties and functionality of the base `Application` class
 * and adds  a rendering layer, called `ViewerLayer`, which is responsible for rendering the 3D scene.
 *
 * Copying or moving `ViewerApp` objects is disabled to ensure single ownership and prevent unintended
 * duplication.
 */
class ViewerApp : public pixc::Application
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    ViewerApp(const std::string &name = "3D Viewer Application", const int width = 800,
              const int height = 600);
    ~ViewerApp();
    
    // Viewer application variables
    // ----------------------------------------
private:
    ///< 3D viewer (rendering layer).
    std::shared_ptr<pixc::RenderingLayer> m_Renderer;
    std::shared_ptr<pixc::GuiLayer> m_Gui;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(ViewerApp);
};
