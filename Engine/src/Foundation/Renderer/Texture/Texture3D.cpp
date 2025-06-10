#include "enginepch.h"
#include "Common/Renderer/Texture/Texture3D.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Texture/OpenGLTexture3D.h"
#include "Platform/Metal/Texture/MetalTexture3D.h"

/**
 * Create a 3D texture based on the active rendering API.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture3D> Texture3D::Create()
{
    CREATE_RENDERER_OBJECT(Texture3D)
}

/**
 * Create a 3D texture based on the active rendering API.
 *
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture3D> Texture3D::Create(const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(Texture3D, spec)
}

/**
 * Create a 3D texture based on the active rendering API.
 *
 * @param data The data for the 3D texture.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture3D> Texture3D::CreateFromData(const void *data)
{
    CREATE_RENDERER_OBJECT(Texture3D, data)
}

/**
 * Create a 3D texture based on the active rendering API.
 *
 * @param data The data for the 3D texture.
 * @param spec The texture specifications.
 *
 * @return A shared pointer to the created texture, or nullptr if the API is not supported or an error occurs.
 */
std::shared_ptr<Texture3D> Texture3D::CreateFromData(const void *data,
                                                     const TextureSpecification& spec)
{
    CREATE_RENDERER_OBJECT(Texture3D, data, spec)
}
