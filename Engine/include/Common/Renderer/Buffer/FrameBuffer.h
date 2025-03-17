#pragma once

#include "Common/Core/Library.h"

#include "Common/Renderer/Buffer/Buffer.h"

#include "Common/Renderer/Texture/Texture.h"
#include "Common/Renderer/Texture/TextureUtils.h"

/**
 * Defines the specification for framebuffer attachments.
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
 * Defines the specifications for a framebuffer.
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
    void SetFrameBufferSize(unsigned int width, unsigned int height = 0,
                            unsigned int depth = 0)
    {
        Width = width;
        Height = height;
        Depth = depth;
    }
    
    // Framebuffer specification variables
    // ----------------------------------------
    ///< The size (width, height, and depth) in pixels.
    unsigned int Width = 0, Height = 0, Depth = 0;
    ///< The number of samples in the framebuffer texture attachments (only valid for 2D textures).
    int Samples = 1;
    ///< A flag indicating whether mipmaps should be created for the texture. 
    bool MipMaps = false;
    
    ///< The properties for framebuffer texture attachments.
    AttachmentSpecification AttachmentsSpec;
};

/**
 * Represents a framebuffer object for rendering off-screen.
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
    const std::shared_ptr<Texture>& GetColorAttachment(const unsigned int index) const
    {
        CORE_ASSERT(index >= 0 && index < m_ColorAttachments.size(),
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
    
    virtual std::vector<char> GetAttachmentData(const unsigned int index) const = 0;
    
    // Usage
    // ----------------------------------------
    virtual void Bind() const;
    virtual void BindForDrawAttachment(const unsigned int index) const = 0;
    virtual void BindForReadAttachment(const unsigned int index) const = 0;
    virtual void BindForDrawAttachmentCube(const unsigned int index, const unsigned int face,
                                           const unsigned int level = 0) const = 0;
    virtual void Unbind(const bool& genMipMaps = true) const = 0;
    
    // Draw
    // ----------------------------------------
    virtual void ClearAttachment(const unsigned int index, const int value) = 0;
    
    // Blit
    // ----------------------------------------
    static void Blit(const std::shared_ptr<FrameBuffer>& src,
                     const std::shared_ptr<FrameBuffer>& dst,
                     const TextureFilter& filter = TextureFilter::Nearest,
                     const RenderTargetBuffers& targets = {});
    static void BlitColorAttachments(const std::shared_ptr<FrameBuffer>& src,
                                     const std::shared_ptr<FrameBuffer>& dst,
                                     const unsigned int srcIndex, const unsigned int dstIndex,
                                     const TextureFilter& filter = TextureFilter::Nearest);
    
    // Reset
    // ----------------------------------------
    void Resize(const unsigned int width, const unsigned int height = 0,
                const unsigned int depth = 0);
    void AdjustSampleCount(const unsigned int samples);
    
    // Save
    // ----------------------------------------
    void SaveAttachment(const unsigned int index, const std::filesystem::path& path);
    
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
 * A library for managing framebuffers used in rendering.
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
