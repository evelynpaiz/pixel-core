#include "pixcpch.h"
#include "Foundation/Renderer/Drawable/Drawable.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/Utils/FactoryUtils.h"

#include "Platform/OpenGL/Drawable/OpenGLDrawable.h"
#ifdef __APPLE__
#include "Platform/Metal/Drawable/MetalDrawable.h"
#endif

namespace pixc {

/**
 * @brief Create a drawable object based on the active rendering API.
 *
 * @return A shared pointer to the created drawable, or `nullptr` if the API
 *         is not supported or an error occurs.
 */
std::shared_ptr<Drawable> Drawable::Create()
{
    CREATE_RENDERER_OBJECT(std::make_shared, Drawable)
}

} // namespace pixc
