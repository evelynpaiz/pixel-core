#include "enginepch.h"
#include "Common/Renderer/Drawable/Drawable.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Drawable/OpenGLDrawable.h"
#include "Platform/Metal/Drawable/MetalDrawable.h"

namespace pixc {

/**
 * @brief Create a drawable object based on the active rendering API.
 *
 * @return A shared pointer to the created drawable, or `nullptr` if the API
 *         is not supported or an error occurs.
 */
std::shared_ptr<Drawable> Drawable::Create()
{
    CREATE_RENDERER_OBJECT(Drawable)
}

} // namespace pixc
