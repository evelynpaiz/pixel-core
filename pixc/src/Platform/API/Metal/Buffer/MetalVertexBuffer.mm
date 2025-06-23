#include "pixcpch.h"
#include "Platform/Metal/Buffer/MetalVertexBuffer.h"

#include "Platform/Metal/MetalContext.h"

#include <Metal/Metal.h>

namespace pixc {

/**
 * @brief Generate a vertex buffer and link it to the input vertex data.
 *
 * @param vertices Vertices to be rendered.
 * @param size Size of vertices in bytes.
 * @param count Number of vertices.
 */
MetalVertexBuffer::MetalVertexBuffer(const void *vertices, const uint32_t size,
                                     const uint32_t count)
    : VertexBuffer(count)
{
    // Get the Metal graphics context
    MetalContext* context = dynamic_cast<MetalContext*>(&GraphicsContext::Get());
    PIXEL_CORE_ASSERT(context, "Graphic context is not Metal!");
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(context->GetDevice());
    
    // Calculate the length of the buffer in bytes
    NSUInteger length = count * size;
    
    // Create the Metal buffer
    id<MTLBuffer> buffer = [device
                            newBufferWithBytes:vertices
                            length:length
                            options:MTLResourceStorageModeShared];
    m_Buffer = reinterpret_cast<void*>(buffer);
}

} // namespace pixc
