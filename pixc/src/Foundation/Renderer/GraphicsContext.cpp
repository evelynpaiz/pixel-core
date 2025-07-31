#include "pixcpch.h"
#include "Foundation/Renderer/GraphicsContext.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/Utils/FactoryUtils.h"

#include "Platform/OpenGL/OpenGLContext.h"
#ifdef __APPLE__
#include "Platform/Metal/MetalContext.h"
#endif

namespace pixc {

// Define static variables
GraphicsContext* GraphicsContext::s_Instance = nullptr;

/**
 * Default constructor for a graphics context.
 */
GraphicsContext::GraphicsContext()
{
    // Define the pointer to the application
    PIXEL_CORE_ASSERT(!s_Instance, "Graphics context already exists!");
    s_Instance = this;
}

/**
 * Creates a graphics context based on the specified window and the active rendering API.
 *
 * @param window A pointer to the window for which to create the graphics context.
 *
 * @return A shared pointer to the created graphics context.
 */
std::unique_ptr<GraphicsContext> GraphicsContext::Create(void *window)
{
    CREATE_RENDERER_OBJECT(std::make_unique, Context, static_cast<GLFWwindow*>(window))
}

/**
 *  Sets the window hints based on the current rendering API.
 */
void GraphicsContext::SetWindowHints()
{
     // Define the window hint for the context creation
     switch (Renderer::GetAPI())
     {
         case RendererAPI::API::None:
             return;
         
         case RendererAPI::API::OpenGL:
             OpenGLContext::SetWindowHints();
             return;
         
         #ifdef __APPLE__
         case RendererAPI::API::Metal:
             MetalContext::SetWindowHints();
             return;
         #endif
     }
     
     PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");
}

} // namespace pixc
