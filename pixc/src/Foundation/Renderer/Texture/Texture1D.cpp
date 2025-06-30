#include "pixcpch.h"
#include "Foundation/Renderer/Texture/Texture1D.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/FactoryUtils.h"

#include "Platform/OpenGL/Texture/OpenGLTexture1D.h"
#ifdef __APPLE__
#include "Platform/Metal/Texture/MetalTexture1D.h"
#endif

namespace pixc {

/**
 * @brief Create a 1D texture based on the active rendering API.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::Create()
{
    CREATE_RENDERER_OBJECT(std::make_shared, Texture1D)
}

/**
 * @brief Create a 1D texture based on the active rendering API.
 *
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::Create(const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(std::make_shared, Texture1D, spec)
}

/**
 * @brief Create a 1D texture based on the active rendering API.
 *
 * @param data The data for the 1D texture.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::CreateFromData(const void *data)
{
    CREATE_RENDERER_OBJECT(std::make_shared, Texture1D, data)
}

/**
 * @brief Create a 1D texture based on the active rendering API.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::CreateFromData(const void *data,
                                                     const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(std::make_shared, Texture1D, data, spec)
}

} // namespace pixc
