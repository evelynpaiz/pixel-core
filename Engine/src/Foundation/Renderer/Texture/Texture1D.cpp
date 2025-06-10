#include "enginepch.h"
#include "Common/Renderer/Texture/Texture1D.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Texture/OpenGLTexture1D.h"
#include "Platform/Metal/Texture/MetalTexture1D.h"

/**
 * Create a 1D texture based on the active rendering API.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::Create()
{
    CREATE_RENDERER_OBJECT(Texture1D)
}

/**
 * Create a 1D texture based on the active rendering API.
 *
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::Create(const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(Texture1D, spec)
}

/**
 * Create a 1D texture based on the active rendering API.
 *
 * @param data The data for the 1D texture.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::CreateFromData(const void *data)
{
    CREATE_RENDERER_OBJECT(Texture1D, data)
}

/**
 * Create a 1D texture based on the active rendering API.
 *
 * @param data The data for the 1D texture.
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture1D> Texture1D::CreateFromData(const void *data,
                                                     const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(Texture1D, data, spec)
}
