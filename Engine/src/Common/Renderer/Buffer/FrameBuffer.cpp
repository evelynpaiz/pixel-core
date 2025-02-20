#include "enginepch.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/Texture/Texture1D.h"
#include "Common/Renderer/Texture/Texture2D.h"
#include "Common/Renderer/Texture/Texture3D.h"
#include "Common/Renderer/Texture/TextureCube.h"

#include "Platform/OpenGL/Buffer/OpenGLFrameBuffer.h"
#include "Platform/Metal/Buffer/MetalFrameBuffer.h"

#include "Common/Renderer/Renderer.h"
#include "Common/Renderer/RendererCommand.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

/**
 * Create a framebuffer based on the active rendering API.
 *
 * @param spec Framebuffer specifications.
 *
 * @return A shared pointer to the created framebuffer, or nullptr if the API
 *         is not supported or an error occurs.
 */
std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
{
    CREATE_RENDERER_OBJECT(FrameBuffer, spec)
}

/**
 * Bind the framebuffer.
 */
void FrameBuffer::Bind() const
{
    RendererCommand::SetViewport(0, 0, m_Spec.Width, m_Spec.Height > 0 ? m_Spec.Height : 1);
}

/**
 * Define the framebuffer.
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
                    utils::textures::IsDepthFormat(spec.Format) ?
                    TextureWrap::ClampToBorder : TextureWrap::ClampToEdge;
        
        // Depth attachment
        if (utils::textures::IsDepthFormat(spec.Format))
        {
            spec.Filter = TextureFilter::Nearest;
            
            // TODO: Add the stencil buffer activation too.
            m_DepthAttachmentSpec = spec;
            m_ActiveTargets.depthBufferActive = true;
        }
        // Color attachment
        else
        {
            spec.Filter = TextureFilter::Linear;
            
            m_ColorAttachmentsSpec.emplace_back(spec);
            m_ActiveTargets.colorBufferActive = true;
        }
    }
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
                       const TextureFilter& filter, const RenderTargetBuffers& targets)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return OpenGLFrameBuffer::Blit(std::dynamic_pointer_cast<OpenGLFrameBuffer>(src),
                                           std::dynamic_pointer_cast<OpenGLFrameBuffer>(dst),
                                           filter, targets);
    }
    CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
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
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return OpenGLFrameBuffer::BlitColorAttachments(std::dynamic_pointer_cast<OpenGLFrameBuffer>(src),
                                                           std::dynamic_pointer_cast<OpenGLFrameBuffer>(dst),
                                                           srcIndex, dstIndex, filter);
    }
    CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

/**
 * Defines the attachments for the framebuffer.
 */
void FrameBuffer::DefineAttachments()
{
    // Define the depth attachment
    if(m_DepthAttachmentSpec.Format != TextureFormat::None &&
       utils::textures::IsDepthFormat(m_DepthAttachmentSpec.Format))
    {
        m_DepthAttachment = Texture2D::Create(m_DepthAttachmentSpec, m_Spec.Samples);
        m_DepthAttachment->CreateTexture(nullptr);
    }
    
    // Define color attachments
    if (m_ColorAttachmentsSpec.empty())
        return;
    
    // Resize the color attachment array based on the number of specifications
    m_ColorAttachments.resize(m_ColorAttachmentsSpec.size());
    
    // Iterate through each color attachment specification
    for (unsigned int i = 0; i < m_ColorAttachments.size(); i++)
    {
        // Get the type and format from the specification
        TextureType &type = m_ColorAttachmentsSpec[i].Type;
        
        // Lambda function to create the texture based on the type
        auto createTexture = [&]() -> std::shared_ptr<Texture> {
            switch (type)
            {
                case TextureType::TEXTURE1D:
                    return Texture1D::Create(m_ColorAttachmentsSpec[i]);
                case TextureType::TEXTURE2D:
                case TextureType::TEXTURE2D_MULTISAMPLE:
                    return Texture2D::Create(m_ColorAttachmentsSpec[i], m_Spec.Samples);
                case TextureType::TEXTURE3D:
                    return Texture3D::Create(m_ColorAttachmentsSpec[i]);
                case TextureType::TEXTURECUBE:
                    return TextureCube::Create(m_ColorAttachmentsSpec[i]);
                case TextureType::None:
                default: return nullptr;
            }
        };
        // Create the texture using the lambda function
        m_ColorAttachments[i] = createTexture();
        
        // Check for errors during texture creation
        if (!m_ColorAttachments[i])
        {
            CORE_WARN("Data in color attachment not properly defined");
            continue;
        }
        
        // Finally, create the texture data
        m_ColorAttachments[i]->CreateTexture(nullptr);
    }
}

/**
 * Releases the resources associated with the framebuffer.
 */
void FrameBuffer::ReleaseFramebuffer()
{
    m_ColorAttachments.clear();
    m_DepthAttachment = nullptr;
}
