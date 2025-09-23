#pragma once

#include "Foundation/Core/ClassUtils.h"
#include "Foundation/Renderer/Texture/TextureUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Specifies filtering modes for minification, magnification, and mipmapping.
 *
 * The `TextureFilterSpec` struct holds separate filter modes for minification, magnification,
 * and mipmap sampling. This allows detailed control over how textures are sampled at different
 * scales and mipmap levels.
 */
struct TextureFilterModes
{
    TextureFilter Min = TextureFilter::None;
    TextureFilter Mag = TextureFilter::None;
    TextureFilter Mip = TextureFilter::None;
};

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
    TextureSpecification(const TextureFormat& format) :
        Format(format)
    {}
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Define the size of the texture (in pixels).
    /// @param width The texture size (width).
    /// @param height The texture size (height)
    void SetTextureSize(uint32_t width, uint32_t height = 0,
                        uint32_t depth = 0)
    {
        Width = width;
        Height = height;
        Depth = depth;
    }
    /// @brief Sets both the minification and magnification filter modes to the same value.
    /// @param filter The filter mode to use for minification and magnification.
    void SetMinMagFilter(TextureFilter filter)
    {
        Filter.Min = filter;
        Filter.Mag = filter;
    }
    /// @brief Sets the mipmap filter mode.
    /// @param filter The filter mode to use for mipmapping.
    void SetMipFilter(TextureFilter filter)
    {
        if (!MipMaps)
            PIXEL_CORE_WARN("Attempting to set mipmap filter while mipmaps are disabled!");
        
        Filter.Mip = filter;
    }
    
    // Texture specification variables
    // ----------------------------------------
    ///< The size (width and height) in pixels.
    uint32_t Width = 0, Height = 0, Depth = 0;
    
    ///< The type (dimension) of the texture.
    TextureType Type = TextureType::None;
    ///< The internal format of the texture data.
    TextureFormat Format = TextureFormat::None;
    
    ///< The texture filtering mode, specifying how the texture is sampled during rendering.
    TextureFilterModes Filter;
    ///< The texture wrap mode, specifying how texture coordinates outside the [0, 1] range
    ///< are handled.
    TextureWrap Wrap = TextureWrap::None;
    
    ///< A flag indicating whether mipmaps should be created for the texture. Mipmaps are
    ///< precalculated versions of the texture at different levels of detail, providing smoother
    ///< rendering at varying distances.
    bool MipMaps = true;
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Equality operator for comparing two texture specifications.
    /// @param other Another texture specification to compare against.
    /// @return True if both specifications match.
    inline bool operator==(const TextureSpecification& other) const
    {
        return Width == other.Width &&
               Height == other.Height &&
               Depth == other.Depth &&
               Type == other.Type &&
               Format == other.Format &&
               Filter.Min == other.Filter.Min &&
               Filter.Mag == other.Filter.Mag &&
               Filter.Mip == other.Filter.Mip &&
               Wrap == other.Wrap &&
               MipMaps == other.MipMaps;
    }
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
    
    virtual int GetChannels() const;
    int GetAlignedChannels() const;
    int GetStride() const;
    
    // Update
    // ----------------------------------------
    void Update(const uint32_t width, const uint32_t height,
                const uint32_t channels, const std::string& extension = "");
    
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
 * @namespace utils::textures
 * @brief Utility functions related to texture operations.
 */
namespace utils { namespace textures {

/**
 * @brief Helper struct for `TextureSpecification`.
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
    static void SetSize(TextureSpecification& spec, uint32_t size);
};

/**
 * @brief Generates a function to get a shared pointer to a cached white texture.
 *
 * @param TextureType The texture type (e.g., `Texture2D`, `Texture3D`).
 *
 * @code{.cpp}
 *   CREATE_WHITE_TEXTURE(Texture2D) // Defines `WhiteTexture2D()`
 * @endcode
 */
#define DEFINE_WHITE_TEXTURE(TextureType)                           \
    inline std::shared_ptr<TextureType>& White##TextureType()       \
    {                                                               \
        static std::shared_ptr<TextureType> texture;                \
        if (texture)                                                \
            return texture;                                         \
        TextureSpecification spec;                                  \
        TextureHelper<TextureType>::SetSize(spec, 1);               \
        spec.Format = TextureFormat::RGB8;                          \
        spec.SetMinMagFilter(TextureFilter::Nearest);               \
        spec.Wrap = TextureWrap::Repeat;                            \
        spec.MipMaps = false;                                       \
        const unsigned char whitePixel[] = {255, 255, 255};         \
        texture = TextureType::CreateFromData(whitePixel, spec);    \
        return texture;\
    }

} // namespace textures
} // namespace utils
} // namespace pixc

namespace std {

/**
 * @brief Hash function specialization for `TextureSpecification`.
 *
 * @note: Allows `MetalRendererDescriptor` to be used as a key in unordered_map.
 */
template<>
struct hash<pixc::TextureSpecification>
{
    /// @brief Generates a hash for a given `TextureSpecification`.
    /// @param key The descriptor to hash.
    /// @return A combined hash of each specification hash.
    size_t operator()(const pixc::TextureSpecification& key) const
    {
        size_t h = std::hash<int>()(key.Width);
        h ^= std::hash<int>()(key.Height) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(key.Depth) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Type)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Format)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Filter.Min)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Filter.Mag)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Filter.Mip)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int>()(static_cast<int>(key.Wrap)) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<bool>()(key.MipMaps) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

} // namespace std
