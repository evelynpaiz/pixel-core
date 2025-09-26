#include "pixcpch.h"
#include "Foundation/Layer/Gui/GuiBackend.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/Utils/FactoryUtils.h"

#include "Platform/OpenGL/Gui/OpenGLGuiBackend.h"
#ifdef __APPLE__
#include "Platform/Metal/Gui/MetalGuiBackend.h"
#endif

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
    CREATE_RENDERER_OBJECT(std::make_unique, GuiBackend)
}

} // namespace pixc
