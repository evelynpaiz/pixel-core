#pragma once

#include "Common/Renderer/Buffer/FrameBuffer.h"

class MetalFrameBuffer : public FrameBuffer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalFrameBuffer(const FrameBufferSpecification& spec);
    ~MetalFrameBuffer() override;
    
    // Getter(s)
    // ----------------------------------------
    std::vector<char> GetAttachmentData(const unsigned int index) override { return std::vector<char>(); };
    
    // Usage
    // ----------------------------------------
    void BindForDrawAttachment(const unsigned int index) const override {};
    void BindForReadAttachment(const unsigned int index) const override {};
    void BindForDrawAttachmentCube(const unsigned int index, const unsigned int face,
                                   const unsigned int level = 0) const override {};
    void Unbind(const bool& genMipMaps = true) const override {};
    
    // Draw
    // ----------------------------------------
    void ClearAttachment(const unsigned int index, const int value) override {};
     
    
    // Blit
    // ----------------------------------------
    static void Blit(const std::shared_ptr<MetalFrameBuffer>& src,
                     const std::shared_ptr<MetalFrameBuffer>& dst,
                     const TextureFilter& filter = TextureFilter::Nearest,
                     const RenderTargetBuffers& targets = {}) {};
    static void BlitColorAttachments(const std::shared_ptr<MetalFrameBuffer>& src,
                                     const std::shared_ptr<MetalFrameBuffer>& dst,
                                     const unsigned int srcIndex, const unsigned int dstIndex,
                                     const TextureFilter& filter = TextureFilter::Nearest) {};
    
    // Save
    // ----------------------------------------
    void SaveAttachment(const unsigned int index,
                        const std::filesystem::path& path) override {};
    
private:
    // Reset
    // ----------------------------------------
    void Invalidate() override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalFrameBuffer);
};
