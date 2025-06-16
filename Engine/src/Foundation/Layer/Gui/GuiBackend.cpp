#include "enginepch.h"
#include "Foundation/Layer/Gui/GuiBackend.h"

#include "Platform/OpenGL/Gui/OpenGLGuiBackend.h"
#include "Platform/Metal/Gui/MetalGuiBackend.h"

#include "Foundation/Renderer/Renderer.h"

namespace pixc {

/**
 * @brief Create the appropriate ImGui backend based on the active Renderer API.
 *
 * @return A unique pointer to the corresponding GuiBackend implementation.
 *
 * @note If the Renderer API is not supported or unknown, this function will trigger an assertion and return nullptr.
 */
std::unique_ptr<GuiBackend> GuiBackend::Create()
{
    
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            PIXEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        
        case RendererAPI::API::OpenGL:
            return std::make_unique<OpenGLGuiBackend>();
        
        #ifdef __APPLE__
        case RendererAPI::API::Metal:
            return std::make_unique<MetalGuiBackend>();
        #endif
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

} // namespace pixc
