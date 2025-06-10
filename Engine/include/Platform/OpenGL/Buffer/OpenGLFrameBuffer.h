#pragma once

#include "Common/Renderer/Buffer/FrameBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
//namespace pixc {

/**
 * @brief Provides an implementation of the `FrameBuffer` interface using the OpenGL API.
 *
 * It handles the creation and management of OpenGL framebuffer objects, color and depth textures,
 * and supports standard OpenGL framebuffer operations.
 *
 * Copying or moving `OpenGLFrameBuffer` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class OpenGLFrameBuffer : public FrameBuffer
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    OpenGLFrameBuffer(const FrameBufferSpecification& spec);
    ~OpenGLFrameBuffer() override;
    
    // Getter(s)
    // ----------------------------------------
    std::vector<char> GetAttachmentData(const uint32_t index) const override;
    
    // Usage
    // ----------------------------------------
    void Bind() const override;
    void BindForDrawAttachment(const uint32_t index) const override;
    void BindForReadAttachment(const uint32_t index) const override;
    void BindForDrawAttachmentCube(const uint32_t index, const uint32_t face,
                                   const uint32_t level = 0) const override;
    void Unbind(const bool& genMipMaps = true) const override;
    
    // Draw
    // ----------------------------------------
    void ClearAttachment(const uint32_t index, const int value) override;
    
    // Blit
    // ----------------------------------------
    static void Blit(const std::shared_ptr<OpenGLFrameBuffer>& src,
                     const std::shared_ptr<OpenGLFrameBuffer>& dst,
                     const BlitSpecification& spec);
    
    private:
    // Destructor
    // ----------------------------------------
    void ReleaseFramebuffer() override;
    
    // Reset
    // ----------------------------------------
    void Invalidate() override;
    
    // Framebuffer variables
    // ----------------------------------------
    private:
    ///< ID of the framebuffer.
    uint32_t m_ID = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(OpenGLFrameBuffer);
};

//} // namespace pixc
