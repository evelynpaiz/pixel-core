#include "enginepch.h"
#include "Foundation/Renderer/RendererAPI.h"

#include "Foundation/Renderer/GraphicsContext.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

#ifdef __APPLE__
#include "Platform/Metal/MetalRendererAPI.h"
#endif

namespace pixc {

// Define static variables
RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

/**
 * Creates a new Renderer API instance based on the selected API.
 *
 * @return A unique pointer to the created RendererAPI instance.
 */
std::unique_ptr<RendererAPI> RendererAPI::Create()
{
    switch (s_API)
    {
        case RendererAPI::API::None:
            PIXEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
            
        case RendererAPI::API::OpenGL:
            return std::make_unique<OpenGLRendererAPI>();
            
#ifdef __APPLE__
        case RendererAPI::API::Metal:
            return std::make_unique<MetalRendererAPI>();
#endif
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

} // namespace pixc
