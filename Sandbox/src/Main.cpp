#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif

#include "Common/Core/Log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "Engine.h"
//#include "Viewer/ViewerApp.h"

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
    Log::Init();
    CORE_INFO("Test passed info!");
    CORE_DEBUG("Test passed debug!");
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Pixel Core", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    
    // Create the application
    //auto application = std::make_unique<ViewerApp>("3D Viewer", 800, 600);
    //application->Run();
}
