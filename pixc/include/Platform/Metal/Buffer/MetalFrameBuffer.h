#pragma once

#include "Foundation/Renderer/Buffer/FrameBuffer.h"

#include "Platform/Metal/MetalContext.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Provides an implementation of the `FrameBuffer` interface using Apple's Metal API.
 *
 * It manages Metal-specific resources for color and depth attachments, binding operations,
 * and framebuffer lifecycle management.
 *
 * Copying or moving `MetalFrameBuffer` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class MetalFrameBuffer : public FrameBuffer
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalFrameBuffer(const FrameBufferSpecification& spec);
    ~MetalFrameBuffer() override;
    
    // Getter(s)
    // ----------------------------------------
    std::vector<char> GetAttachmentData(const uint32_t index) const override;
    
    // Usage
    // ----------------------------------------
    void BindForDrawAttachment(const uint32_t index) const override {};
    void BindForReadAttachment(const uint32_t index) const override {};
    void BindForDrawAttachmentCube(const uint32_t index, const uint32_t face,
                                   const uint32_t level = 0) const override {};
    
    void Unbind(const bool& genMipMaps = true) const override;
    
    // Draw
    // ----------------------------------------
    void ClearAttachment(const uint32_t index, const int value) override {};
    
    
    // Blit
    // ----------------------------------------
    static void Blit(const std::shared_ptr<MetalFrameBuffer>& src,
                     const std::shared_ptr<MetalFrameBuffer>& dst,
                     const BlitSpecification& spec);
    
    private:
    static void BlitDirect(const std::shared_ptr<MetalFrameBuffer>& src,
                           const std::shared_ptr<MetalFrameBuffer>& dst,
                           uint32_t srcIndex, uint32_t dstIndex);
    static void BlitAdvanced(const std::shared_ptr<MetalFrameBuffer>& src,
                             const std::shared_ptr<MetalFrameBuffer>& dst,
                             const BlitSpecification& spec);
    
    // Reset
    // ----------------------------------------
    void Invalidate() override;
    
    // Framebuffer variables
    // ----------------------------------------
    private:
    ///< Metal context.
    MetalContext* m_Context;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(MetalFrameBuffer);
};

} // namespace pixc
