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
 * Unbind the framebuffer and generate the mipmaps if necessary.
 *
 * @param genMipMaps Mip map generation flag.
 */
void MetalFrameBuffer::Unbind(const bool& genMipMaps) const
{
    // Verify if the mip maps are necessary
    if (!m_Spec.MipMaps || !genMipMaps)
        return;
    
    // Generate mip maps
    for (auto& attachment : m_ColorAttachments)
    {
        auto texture = std::dynamic_pointer_cast<MetalTexture>(attachment);
        texture->MTLGenerateMipMaps(false);
    }
}

/**
 * Blit the contents of a source framebuffer to a destination framebuffer.
 *
 * @param src The source framebuffer from which to copy the contents.
 * @param dst The destination framebuffer to which the contents are copied.
 * @param spec The blit-specific parameters such as filter type, target buffers, and attachment indices.
 */
void MetalFrameBuffer::Blit(const std::shared_ptr<MetalFrameBuffer>& src,
                            const std::shared_ptr<MetalFrameBuffer>& dst,
                            const BlitSpecification& spec)
{
    // Ensure that source and destination framebuffers are defined
    CORE_ASSERT(src && dst, "Trying to blit undefined framebuffer(s)");
    
    // Ensure that the source attachment index is valid
    CORE_ASSERT(spec.SrcAttachmentIndex < src->GetSpec().AttachmentsSpec.TexturesSpec.size(),
                "Invalid source color attachment index!");
    CORE_ASSERT(spec.DstAttachmentIndex < dst->GetSpec().AttachmentsSpec.TexturesSpec.size(),
                "Invalid destination color attachment index!");
    
    // Perform a direct copy if no filtering or complex target buffers are involved
    if (spec.Filter == TextureFilter::Nearest && spec.Targets == RenderTargetBuffers())
        BlitDirect(src, dst, spec.SrcAttachmentIndex, spec.DstAttachmentIndex);
    // Otherwise, perform the advanced blit operation with extra parameters
    else
        BlitAdvanced(src, dst, spec);
}

/**
 * Perform a simple, direct blit of color attachments from the source framebuffer to the destination framebuffer.
 *
 * @param src The source framebuffer from which to copy the color attachment.
 * @param dst The destination framebuffer to which the color attachment is copied.
 * @param srcIndex The index of the color attachment in the source framebuffer.
 * @param dstIndex The index of the color attachment in the destination framebuffer.
 */
void MetalFrameBuffer::BlitDirect(const std::shared_ptr<MetalFrameBuffer> &src,
                                  const std::shared_ptr<MetalFrameBuffer> &dst,
                                  unsigned int srcIndex, unsigned int dstIndex)
{
    // Get the graphics context
    MetalContext* context = dynamic_cast<MetalContext*>(&GraphicsContext::Get());
    CORE_ASSERT(context, "Graphics context is not Metal!");
    
    // Ensure the render command buffer is valid and not encoding anything
    CORE_ASSERT(context->GetCommandBuffer(), "Command buffer is null!");
    CORE_ASSERT(!context->GetEncoder(), "Command buffer is still encoding!");
    
    // Get the existing render command buffer
    id<MTLCommandBuffer> commandBuffer = reinterpret_cast<id<MTLCommandBuffer>>(context->GetCommandBuffer());
    
    // Create a blit command encoder
    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    
    // Get the color attachments from both framebuffers
    auto srcAttachment = std::dynamic_pointer_cast<MetalTexture>(src->GetColorAttachment(srcIndex));
    auto dstAttachment = std::dynamic_pointer_cast<MetalTexture>(dst->GetColorAttachment(dstIndex));
    
    id<MTLTexture> srcTexture = reinterpret_cast<id<MTLTexture>>(srcAttachment->MTLGetTexture());
    id<MTLTexture> dstTexture = reinterpret_cast<id<MTLTexture>>(dstAttachment->MTLGetTexture());
    
    // Define the source and destination regions (full framebuffer size)
    MTLOrigin srcOrigin = { 0, 0, 0 };
    MTLSize srcSize = { static_cast<NSUInteger>(src->m_Spec.Width),
                        static_cast<NSUInteger>(src->m_Spec.Height), 1 };

    MTLOrigin dstOrigin = { 0, 0, 0 };

    // Perform the blit operation
    [blitEncoder copyFromTexture:srcTexture
                        sourceSlice:0
                        sourceLevel:0
                        sourceOrigin:srcOrigin
                        sourceSize:srcSize
                        toTexture:dstTexture
                        destinationSlice:0
                        destinationLevel:0
                        destinationOrigin:dstOrigin
    ];

    // End encoding
    [blitEncoder endEncoding];
}

// TODO: should the mip maps be generated when blit?
// TODO: define this method considering:
// - it is a render pass and it has to write to the correct attachment
// - it has to apply the texture filter
// - define a viewport to render into
void MetalFrameBuffer::BlitAdvanced(const std::shared_ptr<MetalFrameBuffer> &src,
                                    const std::shared_ptr<MetalFrameBuffer> &dst,
                                    const BlitSpecification &spec)
{
    CORE_WARN("Blit advanced is not defined!");
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
