#include "pixcpch.h"
#include "Foundation/Renderer/Buffer/IndexBuffer.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/Utils/FactoryUtils.h"

#include "Platform/OpenGL/Buffer/OpenGLIndexBuffer.h"
#ifdef __APPLE__
#include "Platform/Metal/Buffer/MetalIndexBuffer.h"
#endif

namespace pixc {

/**
 * @brief Create a vertex buffer based on the active rendering API.
 *
 * @param indices Index information for the vertices.
 * @param count Number of indices.
 *
 * @return A shared pointer to the created index buffer, or nullptr if the API
 *         is not supported or an error occurs.
 */
std::shared_ptr<IndexBuffer> IndexBuffer::Create(const uint32_t *indices,
                                                 const uint32_t count)
{
    CREATE_RENDERER_OBJECT(std::make_shared, IndexBuffer, indices, count)
}

} // namespace pixc
