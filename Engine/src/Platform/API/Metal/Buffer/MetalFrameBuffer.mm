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
