#pragma once

#include "Foundation/Renderer/Texture/Texture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

// Forward declarations
class Texture2D;

/**
 * @namespace utils::draw
 * @brief Utility functions related to rendering UI and drawing operations.
 */
namespace utils { namespace draw {

inline bool TextureLoader(std::shared_ptr<Texture2D> &texture,
                          std::filesystem::path &name, const char *label,
                          const char *filter, const bool &flip);
} // namespace draw
} // namespace utils

/**
 * @brief Represents a two-dimensional texture.
 *
 * The `Texture2D` class specializes the `Texture` base class to provide functionality
 * for creating and managing standard 2D textures. This class supports multisampling to improve
 * visual quality.
 *
 * Like other texture types, `Texture2D` objects can be bound to specific texture units for
 * use within shaders.
 *
 * @note Copying and moving `Texture2D` objects is disabled to ensure single ownership
 * and prevent unintended resource duplication.
 */
class Texture2D : public Texture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    static std::shared_ptr<Texture2D> Create(uint8_t samples = 1);
    static std::shared_ptr<Texture2D> Create(const TextureSpecification& spec,
                                             uint8_t samples = 1);
    
    static std::shared_ptr<Texture2D> CreateFromData(const void *data, uint8_t samples = 1);
    static std::shared_ptr<Texture2D> CreateFromData(const void *data, const TextureSpecification& spec,
                                                     uint8_t samples = 1);
    
    static std::shared_ptr<Texture2D> CreateFromFile(const std::filesystem::path& filePath,
                                                     bool flip = true);
    static std::shared_ptr<Texture2D> CreateFromFile(const std::filesystem::path& filePath,
                                                     const TextureSpecification& spec,
                                                     bool flip = true);
    
    // Friend class definition(s)
    // ----------------------------------------
    friend bool utils::draw::TextureLoader(std::shared_ptr<Texture2D> &texture,
                                           std::filesystem::path &name, const char *label,
                                           const char *filter, const bool &flip);
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Create a 2D texture with no data.
    /// @param samples The number of samples to use for multisampling.
    Texture2D(uint8_t samples) : Texture(), m_Samples(samples) {}
    /// @brief Create a 2D texture with specific properties and no data.
    /// @param spec The texture specifications.
    /// @param samples The number of samples to use for multisampling.
    Texture2D(const TextureSpecification& spec, uint8_t samples)
    : Texture(spec), m_Samples(samples)
    {}
    
    /// @brief Create a general texture from a specific path.
    /// @param filePath Texture file path.
    /// @param flip Fip the texture vertically.
    Texture2D(const std::filesystem::path& filePath, bool flip) :
    Texture(filePath), m_Flip(flip)
    {}
    /// @brief Create a general texture with specific properties and defined file path.
    /// @param filePath Texture file path.
    /// @param spec The texture specifications.
    /// @param flip Fip the texture vertically.
    Texture2D(const std::filesystem::path& filePath,
              const TextureSpecification& spec, bool flip) :
    Texture(filePath, spec), m_Flip(flip)
    {}
    
protected:
    // Loading
    // ----------------------------------------
    void LoadFromFile(const std::filesystem::path& filePath);
    
    // Texture variables
    // ----------------------------------------
protected:
    ///< The number of samples in the texture.
    uint8_t m_Samples = 1;
    ///< Texture flipping.
    bool m_Flip = true;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Texture2D);
};

/**
 * @namespace utils::textures
 * @brief Utility functions related to texture operations.
 */
namespace utils { namespace textures {

template <>
struct TextureHelper<Texture2D>
{
    /// @brief Sets the width and height of a `TextureSpecification` for a `Texture2D`.
    /// @param spec The `TextureSpecification` object whose size needs to be set.
    /// @param size The width and height to set for the texture.
    static void SetSize(TextureSpecification& spec, unsigned int size)
    {
        spec.SetTextureSize(size, size);
    }
};

/**
 * @brief Get a shared pointer to a 1x1 white texture.
 *
 * @return A 1x1 white texture.
 */
DEFINE_WHITE_TEXTURE(Texture2D)

/**
 * @brief Get a shared pointer to an empty texture with a checkerboard pattern.
 *
 * This function returns a shared pointer to an empty texture with a checkerboard pattern.
 * If the texture has already been created, it will be reused to avoid redundant texture creation.
 *
 * @return A shared pointer to the empty texture with a checkerboard pattern.
 */
inline std::shared_ptr<Texture2D> EmptyTexture2D()
{
    // Static variable to hold the shared pointer to the texture
    static std::shared_ptr<Texture2D> texture;
    
    // Check if the texture has already been created
    if (texture)
        return texture;
    
    // Define a basic filtering
    TextureSpecification spec;
    spec.Filter = TextureFilter::Nearest;
    
    // Create the empty texture using a ccheckerboard pattern
    texture = Texture2D::CreateFromFile("resources/common/checkerboard.png",
                                        spec);
    
    // Return the created texture
    return texture;
}

} // namespace textures
} // namespace utils
} // namespace pixc
