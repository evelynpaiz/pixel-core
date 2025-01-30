#pragma once

#include "Common/Renderer/Texture/TextureUtils.h"

/**
 * Specifications (properties) of a texture.
 *
 * The `TextureSpecification` provides a set of properties to define the characteristics of a
 * texture. These specifications include the size (width and height), the internal format of the texture
 * data, the texture wrap mode, the texture filtering mode, the number of samples (for multisampling),
 * and whether mipmaps should be created for the texture.
 */
struct TextureSpecification
{
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a texture with a default format.
    TextureSpecification() = default;
    
    /// @brief Define a texture with a specific format.
    /// @param type The type of the texture.
    /// @param format The texture format.
    TextureSpecification(const TextureType& type, const TextureFormat& format) :
        Type(type), Format(format)
    {}
    
    /// @brief Define a texture with a specific format.
    /// @param format The texture format.
    /// @param filter The texture sampling filter.
    TextureSpecification(const TextureFormat& format, const TextureFilter& filter) :
        Format(format), Filter(filter)
    { }
    
    /// @brief Define the size of the texture (in pixels).
    /// @param width The texture size (width).
    /// @param height The texture size (height)
    void SetTextureSize(unsigned int width, unsigned int height = 0,
                        unsigned int depth = 0)
    {
        Width = width;
        Height = height;
        Depth = depth;
    }
    
    // Texture specification variables
    // ----------------------------------------
    ///< The size (width and height) in pixels.
    int Width = 0, Height = 0, Depth = 0;
    
    ///< The type (dimension) of the texture.
    TextureType Type = TextureType::None;
    ///< The internal format of the texture data.
    TextureFormat Format = TextureFormat::None;
    
    ///< The texture wrap mode, specifying how texture coordinates outside the [0, 1] range
    ///< are handled.
    TextureWrap Wrap = TextureWrap::None;
    ///< The texture filtering mode, specifying how the texture is sampled during rendering.
    TextureFilter Filter = TextureFilter::None;
    
    ///< A flag indicating whether mipmaps should be created for the texture. Mipmaps are
    ///< precalculated versions of the texture at different levels of detail, providing smoother
    ///< rendering at varying distances.
    bool MipMaps = false;
};

// Forward declarations
class OpenGLFrameBuffer;

/**
 * Abstract base class representing a texture resource.
 *
 * The `Texture` class provides a common interface for creating, binding, and configuring texture data.
 * It serves as an abstract base class, defining the essential operations that concrete texture types (e.g.,
 * `Texture2D`, `TextureCube`) must implement.
 * Textures can be bound to specific texture slots for use in a `Shader`. The class supports
 * loading texture data with specified specifications using the `TextureSpecification` struct.
 *
 * @note Copying and moving `Texture` objects is disabled to ensure single ownership and prevent
 * unintended resource duplication.
 */
class Texture
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the texture.
    virtual ~Texture() = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Bind the texture.
    virtual void Bind() const = 0;
    /// @brief Bind the texture to a specific texture unit.
    /// @param slot The texture unit slot to which the texture will be bound.
    virtual void BindToTextureUnit(uint32_t slot) const = 0;
    /// @brief Unbind the texture.
    virtual void Unbind() const = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the configurations of the texture.
    /// @return The texture specification.
    const TextureSpecification& GetSpecification() const { return m_Spec; }
    
    /// @brief Get the file path of the texture.
    /// @return The path to the file.
    std::filesystem::path GetPath() const { return m_Path; }
    /// @brief Get the name of the loaded texture (file name).
    /// @return The texture name.
    std::string GetName() { return m_Path.filename().string(); }
    /// @brief Get the directory where the texture file is located.
    /// @return The directory of the texture.
    std::string GetDirectory() { return m_Path.parent_path().string(); }
    
    /// @brief Checks if the texture data has been successfully loaded.
    /// @return True if the texture data is loaded, false otherwise.
    bool IsLoaded() const { return m_IsLoaded; }
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class FrameBuffer;
    friend class OpenGLFrameBuffer;
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Create a general texture.
    Texture() = default;
    
    /// @brief Create a general texture with specific properties.
    /// @param spec The texture specifications.
    Texture(const TextureSpecification& spec) : m_Spec(spec) {}
    
    /// @brief Create a general texture from a specific path.
    /// @param path Texture file path.
    Texture(const std::filesystem::path& path) : m_Path(path) {}
    /// @brief Create a general texture with specific properties and defined file path.
    /// @param path Texture file path.
    /// @param spec The texture specifications.
    Texture(const std::filesystem::path& path,
            const TextureSpecification& spec) :
        m_Spec(spec), m_Path(path)
    {}
    
    // Texture creation
    // ----------------------------------------
    virtual void CreateTexture(const void *data) = 0;
    
    // Destructor
    // ----------------------------------------
    virtual void ReleaseTexture() = 0;
    
    // Texture variables
    // ----------------------------------------
protected:
    ///< Texture specifications.
    TextureSpecification m_Spec;
    
    ///< Path to the texture file(s) on disk.
    std::filesystem::path m_Path;
    
    ///< Flag indicating if the texture data has been successfully loaded.
    bool m_IsLoaded = false;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Texture);
};

/**
 * Utility functions related to texture operations.
 */
namespace utils { namespace textures {

/**
 * Helper struct for `TextureSpecification`.
 *
 * @tparam T The texture type (e.g., `Texture2D`, `Texture3D`) that this
 *            specialization of `TextureHelper` will handle.
 */
template <typename T>
struct TextureHelper
{
    /// @brief Sets the size of the `spec` according to the texture type `T`.
    /// @param spec  The `TextureSpecification` object whose size needs to be set.
    /// @param size The size value. 
    static void SetSize(TextureSpecification& spec, unsigned int size);
};

/**
 * Generates a function to get a shared pointer to a cached white texture.
 *
 * @param TextureType The texture type (e.g., `Texture2D`, `Texture3D`).
 *
 * @code{.cpp}
 *   CREATE_WHITE_TEXTURE(Texture2D) // Defines `WhiteTexture2D()`
 * @endcode
 */
#define DEFINE_WHITE_TEXTURE(TextureType)\
    inline std::shared_ptr<TextureType>& White##TextureType()\
    {\
        static std::shared_ptr<TextureType> texture;\
        if (texture)\
            return texture;\
        TextureSpecification spec;\
        TextureHelper<TextureType>::SetSize(spec, 1);\
        spec.Format = TextureFormat::RGB8;\
        spec.Wrap = TextureWrap::Repeat;\
        const unsigned char whitePixel[] = {255, 255, 255};\
        texture = TextureType::CreateFromData(whitePixel, spec);\
        return texture;\
    }

/**
 * Update the specifications of a texture resource based on width, height, channels, and extension.
 *
 * This function updates the specifications of a texture resource (`TextureSpecification`) based on the provided
 * width, height, channels, and extension information. It determines the texture format, wrap mode, filter mode,
 * and whether mipmaps are enabled based on the input data.
 *
 * @param spec The texture specification to be updated.
 * @param width The width of the texture.
 * @param height The height of the texture.
 * @param channels The number of color channels in the texture.
 * @param extension The file extension (e.g., ".hdr") to determine if the texture is HDR.
 */
inline void UpdateSpecsTextureResource(TextureSpecification& spec, const unsigned int width,
                                       const unsigned int height, const unsigned int channels,
                                       const std::string& extension = "")
{
    // Update width and height
    spec.Width = width;
    spec.Height = height;
    
    // Determine if the texture is HDR based on the file extension
    bool isHDR = (extension == ".hdr");
    
    // Define the format of the data to be used
    spec.Format = TextureFormat::None;
    if (!isHDR && channels == 4)
        spec.Format = TextureFormat::RGBA8;
    else if (!isHDR && channels == 3)
        spec.Format = TextureFormat::RGB8;
    else if (isHDR && channels == 3)
        spec.Format = TextureFormat::RGB16F;
    
    // Set default wrap mode based on HDR status
    if (spec.Wrap == TextureWrap::None)
        spec.Wrap = isHDR ? TextureWrap::ClampToEdge : TextureWrap::Repeat;
    
    // Set default filter mode
    if (spec.Filter == TextureFilter::None)
        spec.Filter = TextureFilter::Linear;
    
    // Enable mipmaps by default
    spec.MipMaps = true;
}

} // namespace textures
} // namespace utils
