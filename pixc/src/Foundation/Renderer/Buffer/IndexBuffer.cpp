#include "enginepch.h"
#include "Common/Renderer/Buffer/IndexBuffer.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Buffer/OpenGLIndexBuffer.h"
#include "Platform/Metal/Buffer/MetalIndexBuffer.h"

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
    CREATE_RENDERER_OBJECT(IndexBuffer, indices, count)
}

} // namespace pixc
