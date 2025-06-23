#include "pixcpch.h"
#include "Foundation/Renderer/Buffer/VertexBuffer.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/FactoryUtils.h"

#include "Platform/OpenGL/Buffer/OpenGLVertexBuffer.h"
#include "Platform/Metal/Buffer/MetalVertexBuffer.h"

namespace pixc {

/**
 * @brief Create a vertex buffer based on the active rendering API.
 *
 * @param vertices Vertices to be rendered.
 * @param size Size of vertices in bytes.
 * @param count Number of vertices.
 *
 * @return A shared pointer to the created vertex buffer, or nullptr if the API
 *         is not supported or an error occurs.
 */
std::shared_ptr<VertexBuffer> VertexBuffer::Create(const void* vertices, const uint32_t size,
                                                   const uint32_t count)
{
    CREATE_RENDERER_OBJECT(std::make_shared, VertexBuffer, vertices, size, count)
}

} // namespace pixc
