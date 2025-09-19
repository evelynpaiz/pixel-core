#include "pixcpch.h"
#include "Platform/OpenGL/Buffer/OpenGLFrameBuffer.h"

#include "Foundation/Renderer/Texture/Texture1D.h"
#include "Foundation/Renderer/Texture/Texture2D.h"
#include "Foundation/Renderer/Texture/Texture3D.h"
#include "Foundation/Renderer/Texture/TextureCube.h"

#include "Platform/OpenGL/Texture/OpenGLTexture.h"

#include "Platform/OpenGL/OpenGLRendererUtils.h"
#include "Platform/OpenGL/Texture/OpenGLTextureUtils.h"

#include <stb_image_write.h>

#include <GL/glew.h>

namespace pixc {

/**
 * @brief Generate a framebuffer.
 *
 * @param spec Framebuffer specifications.
 */
OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
: FrameBuffer(spec)
{
    Invalidate();
}

/**
 * @brief Delete the framebuffer.
 */
OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    ReleaseFramebuffer();
}

/**
 * @brief Retrieves pixel data from a color attachment of the framebuffer.
 *
 * @param index The index of the color attachment to retrieve data from.
 *
 * @return A vector containing the pixel data of the color attachment, with each channel.
 */
std::vector<char> OpenGLFrameBuffer::GetAttachmentData(const uint32_t index)
{
    
    // Verify the index for the attachment
    PIXEL_CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds!");
    
    // Get the specifications from the attachment
    TextureSpecification spec = m_ColorAttachments[index]->GetSpecification();
    int stride = m_ColorAttachments[index]->GetStride();
    
    // Create a storage buffer for the texture data
    int imageSize = stride * (spec.Height > 0 ? spec.Height : 1.0f);
    std::vector<char> buffer(imageSize);
    
    // Bind the framebuffer and write the data into the storage buffer
    BindForReadAttachment(index);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    
    glReadPixels(0, 0, spec.Width, (spec.Height > 0 ? spec.Height : 1.0f),
                 utils::textures::gl::ToOpenGLBaseFormat(spec.Format),
                 utils::textures::gl::ToOpenGLDataFormat(spec.Format),
                 buffer.data());
    
    return buffer;
    
    // TODO: Add support for all texture types (currently only supports 1D and 2D).
    // consider using maybe:
    //glGetTexImage(m_ColorAttachments[index]->TextureTarget(), 0,
    //              utils::OpenGL::TextureFormatToOpenGLBaseType(format),
    //              utils::OpenGL::TextureFormatToOpenGLDataType(format),
    //              buffer.data());
}

/**
 * @brief Bind the framebuffer.
 */
void OpenGLFrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
    FrameBuffer::Bind();
}

/**
 * @brief Bind the framebuffer to draw in a specific color attachment.
 *
 * @param index The color attachment index.
 */
void OpenGLFrameBuffer::BindForDrawAttachment(const uint32_t index)
{
    PIXEL_CORE_ASSERT(index < m_ColorAttachments.size(), "Attachment index out of bounds!");
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    FrameBuffer::BindForDrawAttachment(index);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
}

/**
 * @brief Bind the framebuffer to read a specific color attachment.
 *
 * @param index The color attachment index.
 */
void OpenGLFrameBuffer::BindForReadAttachment(const uint32_t index)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID);
    FrameBuffer::BindForReadAttachment(index);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
}

/**
 * @brief Bind the framebuffer to draw in a specific (cube) color attachment.
 *
 * @param index The color attachment index.
 * @param face The face to be selected from the cube attachment.
 * @param level The mipmap level of the texture image to be attached.
 */
void OpenGLFrameBuffer::BindForDrawAttachmentCube(const uint32_t index,
                                                  const uint32_t face,
                                                  const uint32_t level)
{
    auto attachment = std::dynamic_pointer_cast<OpenGLTexture>(m_ColorAttachments[index]);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
    FrameBuffer::BindForDrawAttachmentCube(index, face, level);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                           attachment->m_ID, level);
}

/**
 * @brief Unbind the framebuffer and generate the mipmaps if necessary.
 *
 * @param genMipMaps Mip map generation flag.
 */
void OpenGLFrameBuffer::Unbind(const bool& genMipMaps)
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
    // Reset the draw information
    FrameBuffer::Unbind(genMipMaps);
}

/**
 * @brief Clear a specific attachment belonging to this framebuffer (set a default value on it).
 *
 * @param index Attachment index to be cleared.
 * @param value Clear (reset) value.
 */
void OpenGLFrameBuffer::ClearAttachment(const uint32_t index, const int value)
{
    // TODO: support other types of data. For the moment this is only for RED images.
    auto& spec = m_ColorAttachmentsSpec[index];
    auto attachment = std::dynamic_pointer_cast<OpenGLTexture>(m_ColorAttachments[index]);
    
    glClearTexImage(attachment->m_ID, 0, utils::textures::gl::ToOpenGLInternalFormat(spec.Format),
                    GL_INT, &value);
     
}

/**
 * @brief Blit the contents of a source framebuffer to a destination framebuffer, optionally specifying color attachments.
 *
 * @param src The source framebuffer from which to copy the contents.
 * @param dst The destination framebuffer to which the contents are copied.
 * @param spec The blit-specific parameters such as filter type, target buffers, and attachment indices.
 */
void OpenGLFrameBuffer::Blit(const std::shared_ptr<OpenGLFrameBuffer>& src,
                             const std::shared_ptr<OpenGLFrameBuffer>& dst,
                             const BlitSpecification& spec)
{
    // Ensure that source and destination framebuffers are defined
    PIXEL_CORE_ASSERT(src && dst, "Trying to blit undefined framebuffer(s)");
    
    // Ensure that the source attachment index is valid
    PIXEL_CORE_ASSERT(spec.SrcAttachmentIndex < src->GetSpec().AttachmentsSpec.TexturesSpec.size(),
                "Invalid source color attachment index!");
    PIXEL_CORE_ASSERT(spec.DstAttachmentIndex < dst->GetSpec().AttachmentsSpec.TexturesSpec.size(),
                "Invalid destination color attachment index!");
    
    // Determine the mask based on selected buffer components
    GLbitfield mask = utils::graphics::gl::ToOpenGLClearMask(spec.Targets);
    
    // Bind the source framebuffer and set the read buffer to the specified color attachment
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src->m_ID);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + spec.SrcAttachmentIndex);
    
    // Bind the destination framebuffer and set the draw buffer to the specified color attachment
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->m_ID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + spec.DstAttachmentIndex);
    
    // Perform the blit operation
    glBlitFramebuffer(0, 0, src->m_Spec.Width, src->m_Spec.Height,
                      0, 0, dst->m_Spec.Width, dst->m_Spec.Height,
                      mask, utils::textures::gl::ToOpenGLMagFilter(spec.Filter));
    
    // Unbind the framebuffers and restore the default draw buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
}

/**
 * @brief Define/re-define the framebuffer and its attachments.
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
        auto attachment = std::dynamic_pointer_cast<OpenGLTexture>(m_ColorAttachments[i]);
        
        TextureType type = m_ColorAttachments[i]->GetSpecification().Type;
        GLenum target = utils::textures::gl::ToOpenGLTextureTarget(type);
    
        switch (type)
        {
            case TextureType::TEXTURE1D:
                glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, attachment->m_ID, 0);
                break;
            case TextureType::TEXTURE2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, attachment->m_ID, 0);
                break;
            case TextureType::TEXTURE3D:
                glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, attachment->m_ID, 0, 0);
                break;
            case TextureType::TEXTURECUBE:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, attachment->m_ID, 0);
                break;
            case TextureType::None:
            default:
                break;
        }
    }
    
    // Depth attachment
    if(m_DepthAttachment)
    {
        auto attachment = std::dynamic_pointer_cast<OpenGLTexture>(m_DepthAttachment);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, utils::textures::gl::ToOpenGLDepthAttachment(m_DepthAttachment->m_Spec.Format),
                               utils::textures::gl::ToOpenGLTextureTarget(m_DepthAttachment->m_Spec.Type), attachment->m_ID, 0);
    }
    
    // Draw the color attachments
    if (m_ColorAttachments.size() > 1)
    {
        PIXEL_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Using more than 4 color attachments in the Framebuffer!");
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers((int)m_ColorAttachments.size(), buffers);
    }
    // Only depth-pass
    else if (m_ColorAttachments.empty())
    {
        glDrawBuffer(GL_NONE);
    }
    
    PIXEL_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Releases the resources associated with the framebuffer.
 */
void OpenGLFrameBuffer::ReleaseFramebuffer()
{
    glDeleteFramebuffers(1, &m_ID);
    FrameBuffer::ReleaseFramebuffer();
}

} // namespace pixc
