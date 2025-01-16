#include "enginepch.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/Texture/Texture1D.h"
#include "Common/Renderer/Texture/Texture2D.h"
#include "Common/Renderer/Texture/Texture3D.h"
#include "Common/Renderer/Texture/TextureCube.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <GL/glew.h>

/**
 * Generate a framebuffer.
 *
 * @param spec Framebuffer specifications.
 */
FrameBuffer::FrameBuffer(const FrameBufferSpecification& spec)
    : m_Spec(spec)
{
    // Define the specification for each framebuffer attachment
    for (auto& spec : m_Spec.AttachmentsSpec.TexturesSpec)
    {
        // Update the information of each attachment
        spec.Width = m_Spec.Width;
        spec.Height = m_Spec.Height;
        spec.MipMaps = m_Spec.MipMaps;
        
        spec.Wrap = spec.Wrap != TextureWrap::None ? spec.Wrap :
                    utils::OpenGL::IsDepthFormat(spec.Format) ?
                    TextureWrap::ClampToBorder : TextureWrap::ClampToEdge;
        
        // Depth attachment
        if (utils::OpenGL::IsDepthFormat(spec.Format))
        {
            spec.Filter = TextureFilter::Nearest;
            
            // TODO: Add the stencil buffer activation too.
            m_DepthAttachmentSpec = spec;
            m_ActiveBuffers.depthBufferActive = true;
        }
        // Color attachment
        else
        {
            spec.Filter = TextureFilter::Linear;
            
            m_ColorAttachmentsSpec.emplace_back(spec);
            m_ActiveBuffers.colorBufferActive = true;
        }
    }
    
    // Define the framebuffer along with all its attachments
    Invalidate();
}

/**
 * Delete the framebuffer.
 */
FrameBuffer::~FrameBuffer()
{
    ReleaseFramebuffer();
}

/**
 * Bind the framebuffer.
 */
void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    glViewport(0, 0, m_Spec.Width, m_Spec.Height > 0 ? m_Spec.Height : 1);
}

/**
 * Bind the framebuffer to draw in a specific color attachment.
 *
 * @param index The color attachment index.
 */
void FrameBuffer::BindForDrawAttachment(const unsigned int index) const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    glViewport(0, 0, m_Spec.Width, m_Spec.Height > 0 ? m_Spec.Height : 1);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
}

/**
 * Bind the framebuffer to read a specific color attachment.
 *
 * @param index The color attachment index.
 */
void FrameBuffer::BindForReadAttachment(const unsigned int index) const
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
void FrameBuffer::BindForDrawAttachmentCube(const unsigned int index, const unsigned int face,
                                            const unsigned int level) const
{
    if (m_ColorAttachmentsSpec[index].Type != TextureType::TEXTURECUBE)
    {
        CORE_WARN("Trying to bind for drawing an incorrect attachment type!");
        return;
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    glViewport(0, 0, m_Spec.Width, m_Spec.Height > 0 ? m_Spec.Height : 1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                           m_ColorAttachments[index]->m_ID, level);
}

/**
 * Unbind the vertex buffer.
 */
void FrameBuffer::Unbind(const bool& genMipMaps) const
{
    // Generate mipmaps if necesary
    if (m_Spec.MipMaps && genMipMaps)
    {
        for (auto& attachment : m_ColorAttachments)
        {
            attachment->Bind();
            glGenerateMipmap(attachment->TextureTarget());
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
void FrameBuffer::ClearAttachment(const unsigned int index, const int value)
{
    // TODO: support other types of data. For the moment this is only for RED images.
    auto& spec = m_ColorAttachmentsSpec[index];
    glClearTexImage(m_ColorAttachments[index]->m_ID, 0,
                    utils::OpenGL::TextureFormatToOpenGLInternalType(spec.Format),
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
void FrameBuffer::Blit(const std::shared_ptr<FrameBuffer>& src,
                       const std::shared_ptr<FrameBuffer>& dst,
                       const TextureFilter& filter,
                       const BufferState& buffersActive)
{
    // Ensure that source and destination framebuffers are defined
    CORE_ASSERT(src && dst, "Trying to blit undefined framebuffer(s)");
    
    // Determine the mask based on selected buffer components
    GLbitfield mask = utils::OpenGL::BufferStateToOpenGLMask(buffersActive);
    
    // Bind the source framebuffer for reading and the destination framebuffer for drawing
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->m_ID);
    // Perform the blit operation
    glBlitFramebuffer(0, 0, src->m_Spec.Width, src->m_Spec.Height,
                      0, 0, dst->m_Spec.Width, dst->m_Spec.Height,
                      mask, utils::OpenGL::TextureFilterToOpenGLType(filter, false));
    
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
void FrameBuffer::BlitColorAttachments(const std::shared_ptr<FrameBuffer>& src,
                                       const std::shared_ptr<FrameBuffer>& dst,
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
                      GL_COLOR_BUFFER_BIT, utils::OpenGL::TextureFilterToOpenGLType(filter, false));
    
    // Unbind the framebuffers and restore the default draw buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

/**
 * Reset the size of the framebuffer.
 *
 * @param width Framebuffer width.
 * @param height Famebuffer height.
 */
void FrameBuffer::Resize(const unsigned int width, const unsigned int height,
                         const unsigned int depth)
{
    // Update the size of the framebuffer
    m_Spec.SetFrameBufferSize(width, height, depth);
    
    // Update the size for the framebuffer attachments
    for (auto& spec : m_Spec.AttachmentsSpec.TexturesSpec)
        spec.SetTextureSize(width, height, depth);
    
    for (auto& spec : m_ColorAttachmentsSpec)
        spec.SetTextureSize(width, height, depth);
    
    m_DepthAttachmentSpec.SetTextureSize(width, height, depth);
    
    // Reset the framebuffer
    Invalidate();
}

/**
 * Adjust the sample count of the framebuffer.
 *
 * @param samples New number of samples for multi-sampling.
 */
void FrameBuffer::AdjustSampleCount(const unsigned int samples)
{
    // Update the sample count of the framebuffer
    m_Spec.Samples = samples;
    
    // Reset the framebuffer
    Invalidate();
}

/**
 * Define/re-define the framebuffer and its attachments.
 */
void FrameBuffer::Invalidate()
{
    // Check if framebuffer already exists, if so, delete it
    if (m_ID)
    {
        ReleaseFramebuffer();

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }
    
    // Create the framebuffer
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    
    // Color attachments
    if (!m_ColorAttachmentsSpec.empty())
    {
        // Based on the defined specifications, generate the corresponding attachments
        m_ColorAttachments.resize(m_ColorAttachmentsSpec.size());
        
        for (unsigned int i = 0; i < m_ColorAttachments.size(); i++)
        {
            TextureType &type = m_ColorAttachmentsSpec[i].Type;
            TextureFormat &format = m_ColorAttachmentsSpec[i].Format;
            
            // Define the attachment depending on its type (1D, 2D, 3D, ...)
            auto createTexture = [&]() -> std::shared_ptr<Texture> {
                switch (type)
                {
                    case TextureType::TEXTURE1D: 
                        return std::make_shared<Texture1D>(m_ColorAttachmentsSpec[i]);
                    case TextureType::TEXTURE2D: 
                        return std::make_shared<Texture2D>(m_ColorAttachmentsSpec[i], m_Spec.Samples);
                    case TextureType::TEXTURE3D: 
                        return std::make_shared<Texture3D>(m_ColorAttachmentsSpec[i]);
                    case TextureType::TEXTURECUBE: 
                        return std::make_shared<TextureCube>(m_ColorAttachmentsSpec[i]);
                    case TextureType::None:
                    default: return nullptr;
                }
            };
            m_ColorAttachments[i] = createTexture();
            
            // Check if the attachment has been properly defined
            if (!m_ColorAttachments[i] || format == TextureFormat::None || utils::OpenGL::IsDepthFormat(format))
            {
                CORE_WARN("Data in color attachment not properly defined");
                continue;
            }
            
            // Create the texture for the color attachment
            m_ColorAttachments[i]->CreateTexture(nullptr);
            
            switch (type)
            {
                case TextureType::TEXTURE1D:
                    glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                                           m_ColorAttachments[i]->TextureTarget(), m_ColorAttachments[i]->m_ID, 0);
                    break;
                case TextureType::TEXTURE2D:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                                           m_ColorAttachments[i]->TextureTarget(), m_ColorAttachments[i]->m_ID, 0);
                    break;
                case TextureType::TEXTURE3D:
                    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                                           m_ColorAttachments[i]->TextureTarget(), m_ColorAttachments[i]->m_ID, 0, 0);
                    break;
                case TextureType::TEXTURECUBE:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                                           m_ColorAttachments[i]->TextureTarget(), m_ColorAttachments[i]->m_ID, 0);
                    break;
                case TextureType::None:
                default:
                    break;
            }
        }
    }
    
    // Depth attachment
    if(m_DepthAttachmentSpec.Format != TextureFormat::None &&
       utils::OpenGL::IsDepthFormat(m_DepthAttachmentSpec.Format))
    {
        m_DepthAttachment = std::make_shared<Texture2D>(m_DepthAttachmentSpec, m_Spec.Samples);
        m_DepthAttachment->CreateTexture(nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, utils::OpenGL::TextureFormatToOpenGLDepthType(m_DepthAttachment->m_Spec.Format),
                               m_DepthAttachment->TextureTarget(), m_DepthAttachment->m_ID, 0);
    }
    
    // Draw the color attachments
    if (m_ColorAttachments.size() > 1)
    {
        CORE_ASSERT(m_ColorAttachments.size() <= 4,
                    "Using more than 4 color attachments in the Framebuffer!");
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
void FrameBuffer::ReleaseFramebuffer()
{
    glDeleteFramebuffers(1, &m_ID);
    m_DepthAttachment->ReleaseTexture();
    for (auto& attachment : m_ColorAttachments)
        attachment->ReleaseTexture();
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
void FrameBuffer::SaveAttachment(const unsigned int index, const std::filesystem::path &path)
{
    auto& format = m_ColorAttachmentsSpec[index].Format;
    int channels = utils::OpenGL::TextureFormatToChannelNumber(format);
    
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Ensure the number of channel is in a valid range
    CORE_ASSERT((channels > 0 && channels <= 4),
                "Invalid number of channels in the color attachment!");
    
    // Define the buffer to allocate the attachment data
    int stride = channels * m_Spec.Width;
    int bufferSize = stride * m_Spec.Height;
    void* buffer = utils::AllocateBufferForFormat(format, bufferSize);
    
    // Read the pixel data
    BindForReadAttachment(index);
    glPixelStorei(GL_PACK_ALIGNMENT, channels);
    glReadPixels(0, 0, m_Spec.Width, m_Spec.Height,
                 utils::OpenGL::TextureFormatToOpenGLBaseType(format),
                 utils::OpenGL::TextureFormatToOpenGLDataType(format),
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
    
    utils::DeallocateBufferForFormat(format, buffer);
}
