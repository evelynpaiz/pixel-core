#include "enginepch.h"
#include "Common/Renderer/GraphicsContext.h"

#include "Common/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Metal/MetalContext.h"

// Define static variables
GraphicsContext* GraphicsContext::s_Instance = nullptr;

/**
 * Default constructor for a graphics context.
 */
GraphicsContext::GraphicsContext()
{
    // Define the pointer to the application
    CORE_ASSERT(!s_Instance, "Graphics context already exists!");
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
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:    
            CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); 
            return nullptr;
            
        case RendererAPI::API::OpenGL:
            return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
            
#ifdef __APPLE__
        case RendererAPI::API::Metal:
            return std::make_unique<MetalContext>(static_cast<GLFWwindow*>(window));
#endif
    }
    
    CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
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
    
    CORE_ASSERT(false, "Unknown Renderer API!");
}
