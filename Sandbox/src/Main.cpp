#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif

//#include "Engine.h"
#include "Viewer/ViewerApp.h"

#include "Foundation/Core/Log.h"
#include "Foundation/Core/Library.h"

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
    // Initialize the logging system
    pixc::Log::Init();
    
    // Create the application
    auto application = std::make_unique<ViewerApp>("3D Viewer", 800, 600);
    application->Run();
}
