#include "enginepch.h"
#include "Platform/Metal/Buffer/MetalFrameBuffer.h"

#include "Platform/Metal/Texture/MetalTexture.h"

#include <stb_image_write.h>

#include <Metal/Metal.h>

/**
 * Generate a framebuffer.
 *
 * @param spec Framebuffer specifications.
 */
MetalFrameBuffer::MetalFrameBuffer(const FrameBufferSpecification& spec)
    : FrameBuffer(spec)
{
    // Define the framebuffer along with all its attachments
    Invalidate();
    
    // Get the Metal graphics context and save it
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
}

/**
 * Delete the framebuffer.
 */
MetalFrameBuffer::~MetalFrameBuffer()
{
    ReleaseFramebuffer();
}

/**
 * Define/re-define the framebuffer and its attachments.
 */
void MetalFrameBuffer::Invalidate()
{
    // Check if framebuffer already exists, if so, delete it
    ReleaseFramebuffer();
    
    // Define the framebuffer texture(s)
    DefineAttachments();
}

/**
 * Retrieves pixel data from a color attachment of the framebuffer.
 *
 * @param index The index of the color attachment to retrieve data from.
 *
 * @return A vector containing the pixel data of the color attachment, with each channel.
 */
std::vector<char> MetalFrameBuffer::GetAttachmentData(const unsigned int index) const
{
    // Verify the index for the attachment
    CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds!");
    
    // Get the Metal device from the context
    auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
    // Get the command queue dedicated for the resources
    auto queue = reinterpret_cast<id<MTLCommandQueue>>(m_Context->GetResourceQueue());
    
    // Get the color attachment from the framebuffer
    auto texture = std::dynamic_pointer_cast<MetalTexture>(GetColorAttachment(index));
    id<MTLTexture> attachment = reinterpret_cast<id<MTLTexture>>(texture->MTLGetTexture());
    
    // Get the specifications from the attachment
    TextureSpecification spec = m_ColorAttachments[index]->GetSpecification();
    int channels = m_ColorAttachments[index]->GetAlignedChannels();
    int stride = m_ColorAttachments[index]->GetStride();
    
    // Define the size of the buffer that will contain the data
    int imageSize = stride * (spec.Height > 0 ? spec.Height : 1.0f);
    
    // Create a Metal buffer to store data
    id<MTLBuffer> buffer = [device
                                newBufferWithLength:imageSize
                                options:MTLResourceStorageModeShared];
    
    // Create a blit command encoder to copy texture data to buffer
    id<MTLCommandBuffer> commandBuffer = [queue commandBuffer];
    // Create a blit command encoder to copy texture data to buffer
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    
    // Synchronize the texture data
    [blitEncoder synchronizeResource:attachment];
    
    // Blit the framebuffer data into the read buffer
    [blitEncoder copyFromTexture:attachment
                     sourceSlice:0
                     sourceLevel:0
                     sourceOrigin:MTLOriginMake(0, 0, 0)
                     sourceSize:MTLSizeMake(spec.Width, spec.Height, 1)
                     toBuffer:buffer
                     destinationOffset:0
                     destinationBytesPerRow:stride
                     destinationBytesPerImage:imageSize];

    [blitEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    
    // Copy the data from the read buffer into the output
    std::vector<char> data(imageSize);
    memcpy(data.data(), [buffer contents], imageSize);
    return data;
}
