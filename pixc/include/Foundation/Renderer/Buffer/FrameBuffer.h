#pragma once

#include "Foundation/Core/Library.h"

#include "Foundation/Renderer/Buffer/Buffer.h"

#include "Foundation/Renderer/Texture/Texture.h"
#include "Foundation/Renderer/Texture/TextureUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Defines the specification for framebuffer attachments.
 *
 * The `AttachmentSpecification` struct provides a way to define the specifications for
 * framebuffer attachments. It allows specifying one or more `TextureSpecification`
 * objects for the attachments. These texture specifications define the format, size, and other
 * properties of the textures used as attachments in the framebuffer.
 */
struct AttachmentSpecification
{
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a framebuffer attachment with with no texture specifications.
    AttachmentSpecification() = default;
    /// @brief Define a framebuffer attachment with texture specifications.
    /// @param spec The texture specifications.
    AttachmentSpecification(std::initializer_list<TextureSpecification> spec) :
    TexturesSpec(spec)
    { }
    
    // Attachment specification variables
    // ----------------------------------------
    ///< The texture specifications for the framebuffer attachments.
    std::vector<TextureSpecification> TexturesSpec;
};

/**
 * @brief Defines the specifications for a framebuffer.
 *
 * The `FrameBufferSpecification` struct provides a way to define the specifications for
 * a framebuffer. It includes the framebuffer size (width and height) and the number of samples
 * for multisampling, if applicable. The struct also includes an `AttachmentSpecification`
 * object that defines the texture specifications for the framebuffer attachments.
 */
struct FrameBufferSpecification
{
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a framebuffer with a default specification.
    FrameBufferSpecification() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Define the size of the framebuffer (in pixels).
    /// @param width The framebuffer size (width).
    /// @param height The framebuffer size (height)
    void SetFrameBufferSize(uint32_t width, uint32_t height = 0,
                            uint32_t depth = 0)
    {
        Width = width;
        Height = height;
        Depth = depth;
    }
    
    // Framebuffer specification variables
    // ----------------------------------------
    ///< The size (width, height, and depth) in pixels.
    uint32_t Width = 0, Height = 0, Depth = 0;
    ///< The number of samples in the framebuffer texture attachments (only valid for 2D textures).
    uint32_t Samples = 1;
    ///< A flag indicating whether mipmaps should be created for the texture.
    bool MipMaps = false;
    
    ///< The properties for framebuffer texture attachments.
    AttachmentSpecification AttachmentsSpec;
};

/**
 * @brief Defines the specifications for a framebuffer blit operation.
 */
struct BlitSpecification
{
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a blit specification with default values.
    BlitSpecification() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the texture filter to be used for the blit.
    /// @param filter The texture filter for the blit operation.
    void SetFilter(const TextureFilter& filter) { Filter = filter; }
    
    /// @brief Set the color attachments for the blit operation.
    /// @param targets The buffers to be copied during the blit.
    void SetTargets(const RenderTargetBuffers& targets) { Targets = targets; }
    
    /// @brief Set the source and destination attachment indices.
    /// @param srcIndex The source attachment index.
    /// @param dstIndex The destination attachment index.
    void SetAttachmentIndices(uint32_t srcIndex, uint32_t dstIndex)
    {
        SrcAttachmentIndex = srcIndex;
        DstAttachmentIndex = dstIndex;
    }
    
    // Blit specification variables
    // ----------------------------------------
    ///< The texture filter for the blit operation.
    TextureFilter Filter = TextureFilter::Nearest;
    
    ///< The buffers to be copied during the blit operation.
    RenderTargetBuffers Targets;
    
    ///< The index of the source color attachment.
    uint32_t SrcAttachmentIndex = 0;
    
    ///< The index of the destination color attachment.
    uint32_t DstAttachmentIndex = 0;
};

/**
 * @brief Represents a framebuffer object for rendering off-screen.
 *
 * The `FrameBuffer` class provides functionality to create, bind, unbind, and resize a framebuffer.
 * It allows users to attach color and depth textures to the framebuffer for rendering.
 *
 * Copying or moving `FrameBuffer` objects is disabled to ensure single ownership and prevent
 * unintended buffer duplication.
 */
class FrameBuffer
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    static std::shared_ptr<FrameBuffer> Create(const FrameBufferSpecification& spec);
    virtual ~FrameBuffer() = default;
    
    // Getters
    // ----------------------------------------
    /// @brief Get the framebuffer configuration.
    /// @return The specifications of the framebuffer.
    const FrameBufferSpecification& GetSpec() const { return m_Spec; }
    
    /// @brief Get a specific framebuffer color attachment.
    /// @param index Color attachment index.
    /// @return The color attachment (texture reference).
    const std::shared_ptr<Texture>& GetColorAttachment(const uint32_t index) const
    {
        PIXEL_CORE_ASSERT(index >= 0 && index < m_ColorAttachments.size(),
                    "Trying to get color attachment out of scope!");
        return m_ColorAttachments[index];
    }
    /// @brief Get all the color attachments defined in the framebuffer.
    /// @return The color attachments (texture references).
    const std::vector<std::shared_ptr<Texture>>& GetColorAttachments() const
    {
        return m_ColorAttachments;
    }
    /// @brief Get the framebuffer depth attachment.
    /// @return The depth attachment (texture reference).
    const std::shared_ptr<Texture>& GetDepthAttachment() const { return m_DepthAttachment; }
    
    /// @brief Get the active rendering targets for this framebuffer.
    /// @return The state of the color, depth and stencil targets state.
    RenderTargetBuffers GetActiveRenderTargets() const { return m_ActiveTargets; }
    
    virtual std::vector<char> GetAttachmentData(const uint32_t index) const = 0;
    
    // Usage
    // ----------------------------------------
    virtual void Bind() const;
    virtual void BindForDrawAttachment(const uint32_t index) const = 0;
    virtual void BindForReadAttachment(const uint32_t index) const = 0;
    virtual void BindForDrawAttachmentCube(const uint32_t index, const uint32_t face,
                                           const uint32_t level = 0) const = 0;
    
    virtual void Unbind(const bool& genMipMaps = true) const = 0;
    
    // Draw
    // ----------------------------------------
    virtual void ClearAttachment(const uint32_t index, const int value) = 0;
    
    // Blit
    // ----------------------------------------
    static void Blit(const std::shared_ptr<FrameBuffer>& src,
                     const std::shared_ptr<FrameBuffer>& dst,
                     const BlitSpecification& spec = BlitSpecification());
    
    // Reset
    // ----------------------------------------
    void Resize(const uint32_t width, const uint32_t height = 0,
                const uint32_t depth = 0);
    void AdjustSampleCount(const uint32_t samples);
    
    // Save
    // ----------------------------------------
    void SaveAttachment(const uint32_t index, const std::filesystem::path& path);
    
    protected:
    // Constructor(s)/Destructor
    // ----------------------------------------
    FrameBuffer(const FrameBufferSpecification& spec);
    
    // Reset
    // ----------------------------------------
    virtual void Invalidate() = 0;
    void DefineAttachments();
    
    // Destructor
    // ----------------------------------------
    virtual void ReleaseFramebuffer();
    
    // Framebuffer variables
    // ----------------------------------------
    protected:
    ///< Depth attachment.
    std::shared_ptr<Texture> m_DepthAttachment;
    ///< Color attachments.
    std::vector<std::shared_ptr<Texture>> m_ColorAttachments;
    
    ///< Framebuffer properties.
    FrameBufferSpecification m_Spec;
    ///< Color attachments specifications.
    std::vector<TextureSpecification> m_ColorAttachmentsSpec;
    ///< Depth attachment specification.
    TextureSpecification m_DepthAttachmentSpec;
    
    ///< The states active in the framebuffer.
    RenderTargetBuffers m_ActiveTargets = { false, false, false };
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(FrameBuffer);
};

/**
 * @brief A library for managing framebuffers used in rendering.
 *
 * The `FrameBufferLibrary` class provides functionality to add, create, retrieve, and check for
 * the existence of framebuffers within the library. Framebuffers can be associated with unique names
 * for easy access.
 */
class FrameBufferLibrary : public Library<std::shared_ptr<FrameBuffer>>
{
    public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new framebuffer library.
    FrameBufferLibrary() : Library("Frame buffer") {}
    
    // Create
    // ----------------------------------------
    /// @brief Loads a framebuffer and adds it to the library.
    /// @tparam Type The type of object to load.
    /// @tparam Args The types of arguments to forward to the object constructor.
    /// @param name The name to associate with the loaded object.
    /// @param args The arguments to forward to the object constructor.
    /// @return The framebuffer created.
    std::shared_ptr<FrameBuffer> Create(const std::string& name,
                                        FrameBufferSpecification spec)
    {
        auto framebuffer = FrameBuffer::Create(spec);
        Add(name, framebuffer);
        return framebuffer;
    }
};

} // namespace pixc
