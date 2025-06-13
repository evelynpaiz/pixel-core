#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif

//#include "Engine.h"
#include "Viewer/ViewerApp.h"

#include "Foundation/Core/Log.h"
#include "Foundation/Core/Library.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

/**
 * Entry point of the application.
 *
 * The `main` function serves as the entry point of the application. It initializes the logging system,
 * creates an instance of the viewer application and runs it.
 *
 * @return An integer indicating the exit status of the application.
 */
int main()
{
    // TODO: remove
    MTL::Device* pDevice = MTL::CreateSystemDefaultDevice();
    pDevice->release();
    
    // Initialize the logging system
    pixc::Log::Init();
    
    // Create the application
    auto application = std::make_unique<ViewerApp>("3D Viewer", 800, 600);
    application->Run();
}
