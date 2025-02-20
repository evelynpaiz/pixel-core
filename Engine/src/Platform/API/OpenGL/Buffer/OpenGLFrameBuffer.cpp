#include "enginepch.h"
#include "Platform/OpenGL/Buffer/OpenGLFrameBuffer.h"

#include "Common/Renderer/Texture/Texture1D.h"
#include "Common/Renderer/Texture/Texture2D.h"
#include "Common/Renderer/Texture/Texture3D.h"
#include "Common/Renderer/Texture/TextureCube.h"

#include "Platform/OpenGL/Texture/OpenGLTexture.h"

#include "Platform/OpenGL/OpenGLRendererUtils.h"
#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <stb_image_write.h>

#include <GL/glew.h>

/**
 * Generate a framebuffer.
 *
 * @param spec Framebuffer specifications.
 */
OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
    : FrameBuffer(spec)
{
    // Define the framebuffer along with all its attachments
    Invalidate();
}

/**
 * Delete the framebuffer.
 */
OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    ReleaseFramebuffer();
}

/**
 * Retrieves pixel data from a color attachment of the framebuffer.
 *
 * @param index The index of the color attachment to retrieve data from.
 *
 * @return A vector containing the pixel data of the color attachment, with each channel.
 */
std::vector<char> OpenGLFrameBuffer::GetAttachmentData(const unsigned int index)
{
    // Verify the index for the attachment
    CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds!");
    
    auto& format = m_ColorAttachmentsSpec[index].Format;
    int channels = utils::textures::GetChannelCount(format);
    
    // Ensure numChannels is within a valid range
    if (channels < 1 || channels > 4)
        CORE_ASSERT(false, "Invalid number of channels in the color attachment!");
    
    // Define the data container based on the size of the data
    const int bytes = utils::textures::GetBytesPerChannel(format);
    int stride = bytes * channels * m_Spec.Width;
    channels += (stride % 4) ? (4 - stride % 4) : 0;
    int bufferSize = stride * (m_Spec.Height > 0 ? m_Spec.Height : 1.0f);
    std::vector<char> buffer(bufferSize);
    
    BindForReadAttachment(index);
    glPixelStorei(GL_PACK_ALIGNMENT, channels);
    
    glReadPixels(0, 0, m_Spec.Width, (m_Spec.Height > 0 ? m_Spec.Height : 1.0f),
                 utils::textures::gl::ToOpenGLBaseFormat(format),
                 utils::textures::gl::ToOpenGLDataFormat(format),
                 buffer.data());
    
    // TODO: Add support for all texture types (currently only supports 1D and 2D).
    // consider using maybe:
    //glGetTexImage(m_ColorAttachments[index]->TextureTarget(), 0,
    //              utils::OpenGL::TextureFormatToOpenGLBaseType(format),
    //              utils::OpenGL::TextureFormatToOpenGLDataType(format),
    //              buffer.data());
    
    return buffer;
}

/**
 * Bind the framebuffer.
 */
void OpenGLFrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    FrameBuffer::Bind();
}

/**
 * Bind the framebuffer to draw in a specific color attachment.
 *
 * @param index The color attachment index.
 */
void OpenGLFrameBuffer::BindForDrawAttachment(const unsigned int index) const
{
    CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds!");
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    FrameBuffer::Bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
}

/**
 * Bind the framebuffer to read a specific color attachment.
 *
 * @param index The color attachment index.
 */
void OpenGLFrameBuffer::BindForReadAttachment(const unsigned int index) const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
}

/**
 * Bind the framebuffer to draw in a specific (cube) color attachment.
 *
 * @param index The color attachment index.
 * @param face The face to be selected from the cube attachment.
 * @param level The mipmap level of the texture image to be attached.
 */
void OpenGLFrameBuffer::BindForDrawAttachmentCube(const unsigned int index, 
                                                  const unsigned int face,
                                                  const unsigned int level) const
{
    if (m_ColorAttachmentsSpec[index].Type != TextureType::TEXTURECUBE)
    {
        CORE_WARN("Trying to bind for drawing an incorrect attachment type!");
        return;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    FrameBuffer::Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                           OpenGLTexture::GLGetTextureID(m_ColorAttachments[index]),
                           level);
}

/**
 * Unbind the vertex buffer.
 */
void OpenGLFrameBuffer::Unbind(const bool& genMipMaps) const
{
    // Generate mipmaps if necesary
    if (m_Spec.MipMaps && genMipMaps)
    {
        for (auto& attachment : m_ColorAttachments)
        {
            attachment->Bind();
            glGenerateMipmap(utils::textures::gl::ToOpenGLTextureTarget(attachment->m_Spec.Type));
        }
    }
    
    // Bind to the default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Clear a specific attachment belonging to this framebuffer (set a default value on it).
 *
 * @param index Attachment index to be cleared.
 * @param value Clear (reset) value.
 */
void OpenGLFrameBuffer::ClearAttachment(const unsigned int index, const int value)
{
    // TODO: support other types of data. For the moment this is only for RED images.
    auto& spec = m_ColorAttachmentsSpec[index];
    
    glClearTexImage(OpenGLTexture::GLGetTextureID(m_ColorAttachments[index]),
                    0, utils::textures::gl::ToOpenGLInternalFormat(spec.Format),
                    GL_INT, &value);
}

/**
 * Blit the contents of a source framebuffer to a destination framebuffer.
 *
 * @param src The source framebuffer from which to copy the contents.
 * @param dst The destination framebuffer to which the contents are copied.
 * @param filter The filtering method used for the blit operation.
 * @param colorBuffer If true, copy color buffer components.
 * @param depthBuffer If true, copy depth buffer components.
 * @param stencilBuffer If true, copy stencil buffer components.
 */
void OpenGLFrameBuffer::Blit(const std::shared_ptr<OpenGLFrameBuffer>& src,
                             const std::shared_ptr<OpenGLFrameBuffer>& dst,
                             const TextureFilter& filter,
                             const RenderTargetBuffers& targets)
{
    // Ensure that source and destination framebuffers are defined
    CORE_ASSERT(src && dst, "Trying to blit undefined framebuffer(s)");
    
    // Determine the mask based on selected buffer components
    GLbitfield mask = utils::graphics::gl::ToOpenGLClearMask(targets);
    
    // Bind the source framebuffer for reading and the destination framebuffer for drawing
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->m_ID);
    // Perform the blit operation
    glBlitFramebuffer(0, 0, src->m_Spec.Width, src->m_Spec.Height,
                      0, 0, dst->m_Spec.Width, dst->m_Spec.Height,
                      mask, utils::textures::gl::ToOpenGLFilter(filter, false));
    
    // Unbind the framebuffers
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Blit a specific color attachment from a source framebuffer to a destination framebuffer.
 *
 * @param src The source framebuffer from which to copy the color attachment.
 * @param dst The destination framebuffer to which the color attachment is copied.
 * @param srcIndex The index of the color attachment in the source framebuffer.
 * @param dstIndex The index of the color attachment in the destination framebuffer.
 * @param filter The filtering method used for the blit operation.
 */
void OpenGLFrameBuffer::BlitColorAttachments(const std::shared_ptr<OpenGLFrameBuffer>& src,
                                             const std::shared_ptr<OpenGLFrameBuffer>& dst,
                                             const unsigned int srcIndex, const unsigned int dstIndex,
                                             const TextureFilter& filter)
{
    // Bind the source framebuffer and set the read buffer to the specified color attachment
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_ID);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + srcIndex);
    
    // Bind the destination framebuffer and set the draw buffer to the specified color attachment
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->m_ID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + dstIndex);
    
    // Copy the block of pixels from the source to the destination color attachment
    glBlitFramebuffer(0, 0, src->m_Spec.Width, src->m_Spec.Height,
                      0, 0, dst->m_Spec.Width, dst->m_Spec.Height,
                      GL_COLOR_BUFFER_BIT, utils::textures::gl::ToOpenGLFilter(filter, false));
    
    // Unbind the framebuffers and restore the default draw buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

/**
 * Define/re-define the framebuffer and its attachments.
 */
void OpenGLFrameBuffer::Invalidate()
{
    // Check if framebuffer already exists, if so, delete it
    if (m_ID)
        ReleaseFramebuffer();
    
    // Create the framebuffer
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    
    // Define the framebuffer texture(s)
    DefineAttachments();
    
    // Attach defined textures to framebuffer
    for (unsigned int i = 0; i < m_ColorAttachments.size(); i++)
    {
        TextureType type = m_ColorAttachments[i]->GetSpecification().Type;
        GLenum target = utils::textures::gl::ToOpenGLTextureTarget(type);
        
        switch (type)
        {
            case TextureType::TEXTURE1D:
                glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target,
                                       OpenGLTexture::GLGetTextureID(m_ColorAttachments[i]), 0);
                break;
            case TextureType::TEXTURE2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target,
                                       OpenGLTexture::GLGetTextureID(m_ColorAttachments[i]), 0);
                break;
            case TextureType::TEXTURE3D:
                glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target,
                                       OpenGLTexture::GLGetTextureID(m_ColorAttachments[i]), 0, 0);
                break;
            case TextureType::TEXTURECUBE:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target,
                                       OpenGLTexture::GLGetTextureID(m_ColorAttachments[i]), 0);
                break;
            case TextureType::None:
            default:
                break;
        }
    }
    
    // Depth attachment
    if(m_DepthAttachment)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, utils::textures::gl::ToOpenGLDepthAttachment(m_DepthAttachment->m_Spec.Format),
                               utils::textures::gl::ToOpenGLTextureTarget(m_DepthAttachment->m_Spec.Type), OpenGLTexture::GLGetTextureID(m_DepthAttachment), 0);
    }
    
    // Draw the color attachments
    if (m_ColorAttachments.size() > 1)
    {
        CORE_ASSERT(m_ColorAttachments.size() <= 4, "Using more than 4 color attachments in the Framebuffer!");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers((int)m_ColorAttachments.size(), buffers);
    }
    // Only depth-pass
    else if (m_ColorAttachments.empty())
    {
        glDrawBuffer(GL_NONE);
    }
    
    CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * Releases the resources associated with the framebuffer.
 */
void OpenGLFrameBuffer::ReleaseFramebuffer()
{
    glDeleteFramebuffers(1, &m_ID);
    FrameBuffer::ReleaseFramebuffer();
}

/**
 * Save a color attachment into an output file.
 *
 * Reference:
 * https://lencerf.github.io/post/2019-09-21-save-the-opengl-rendering-to-image-file/
 *
 * @param index Index to the color attachment to be saved.
 * @param path File path.
 */
void OpenGLFrameBuffer::SaveAttachment(const unsigned int index,
                                       const std::filesystem::path &path)
{
    auto& format = m_ColorAttachmentsSpec[index].Format;
    int channels = utils::textures::GetChannelCount(format);
    
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Ensure the number of channel is in a valid range
    if (channels < 1 || channels > 4)
        CORE_ASSERT(false, "Invalid number of channels in the color attachment!");
    
    // Define the buffer to allocate the attachment data
    int stride = channels * m_Spec.Width;
    channels += (stride % 4) ? (4 - stride % 4) : 0;
    int bufferSize = stride * m_Spec.Height;
    void* buffer = utils::textures::AllocateTextureData(format, bufferSize);
    
    // Read the pixel data
    BindForReadAttachment(index);
    glPixelStorei(GL_PACK_ALIGNMENT, channels);
    glReadPixels(0, 0, m_Spec.Width, m_Spec.Height,
                 utils::textures::gl::ToOpenGLBaseFormat(format),
                 utils::textures::gl::ToOpenGLDataFormat(format),
                 buffer);

    // TODO: support more file formats
    // Save data into the file
    stbi_flip_vertically_on_write(true);
    
    if (extension == ".png")
        stbi_write_png(path.string().c_str(), m_Spec.Width, m_Spec.Height, channels, buffer, stride);
    else if (extension == ".jpg" || extension == ".jpeg")
        stbi_write_jpg(path.string().c_str(), m_Spec.Width, m_Spec.Height, channels, buffer, 100);  // Quality parameter (0-100)
    else if (extension == ".hdr")
        stbi_write_hdr(path.string().c_str(), m_Spec.Width, m_Spec.Height, channels, (float*)buffer);
    else
        CORE_WARN("Unsupported file format!");
    
    utils::textures::FreeTextureData(format, buffer);
}
