#pragma once

#include "Foundation/Renderer/Texture/Texture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a cube map texture.
 *
 * The `TextureCube` class specializes the `Texture` base class to provide functionality for creating
 * and managing cube map textures. Cube maps consist of six square textures representing the faces of a cube,
 * typically used for environment mapping, skyboxes, and reflection effects.
 *
 * Like other texture types, `TextureCube` objects can be bound to specific texture units for use within
 * shaders.
 *
 * @note Copying or moving `TextureCube` objects is disabled to ensure single ownership and prevent
 * unintended resource duplication.
 */
class TextureCube : public Texture
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    static std::shared_ptr<TextureCube> Create();
    static std::shared_ptr<TextureCube> Create(const TextureSpecification& spec);
    
    static std::shared_ptr<TextureCube> CreateFromData(const void *data);
    static std::shared_ptr<TextureCube> CreateFromData(const std::vector<const void *>& data);
    static std::shared_ptr<TextureCube> CreateFromData(const void *data, const TextureSpecification& spec);
    static std::shared_ptr<TextureCube> CreateFromData(const std::vector<const void *>& data,
                                                       const TextureSpecification& spec);
    
    static std::shared_ptr<TextureCube> CreateFromFile(const std::filesystem::path& directory,
                                                       const std::vector<std::string>& files, bool flip);
    static std::shared_ptr<TextureCube> CreateFromFile(const std::filesystem::path& directory,
                                                       const std::vector<std::string>& files,
                                                       const TextureSpecification& spec,
                                                       bool flip);
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Create a cube texture with no data.
    TextureCube() = default;
    /// @brief Create a cube texture with specific properties and no data.
    /// @param spec The texture specifications.
    TextureCube(const TextureSpecification& spec)
    : Texture(spec)
    {}
    
    /// @brief Create a general texture from a specific path.
    /// @param directory Textures file path.
    /// @param files List of texture files.
    /// @param flip Fip the texture vertically.
    TextureCube(const std::filesystem::path& directory,
                const std::vector<std::string>& files, bool flip) :
    Texture(directory), m_Files(files), m_Flip(flip)
    {}
    /// @brief Create a general texture with specific properties and defined file path.
    /// @param directory Textures file path.
    /// @param files List of texture files.
    /// @param spec The texture specifications.
    /// @param flip Fip the texture vertically.
    TextureCube(const std::filesystem::path& directory,
                const std::vector<std::string>& files,
                const TextureSpecification& spec, bool flip) :
    Texture(directory, spec), m_Files(files), m_Flip(flip)
    {}
    
    // Texture creation
    // ----------------------------------------
    virtual void CreateTexture(const std::vector<const void *>& data) = 0;
    
    // Loading
    // ----------------------------------------
    void LoadFromFile(const std::filesystem::path& directory,
                      const std::vector<std::string>& files);
    
    // Texture variables
    // ----------------------------------------
protected:
    ///< File names.
    std::vector<std::string> m_Files;
    
    ///< Texture flipping.
    bool m_Flip = true;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(TextureCube);
};

/**
 * @namespace utils::textures
 * @brief Utility functions related to texture operations.
 */
namespace utils { namespace textures {

template <>
struct TextureHelper<TextureCube>
{
    /// @brief Sets the width and height of a `TextureSpecification` for the faces of a `TextureCube`.
    /// @param spec The `TextureSpecification` object whose size needs to be set.
    /// @param size The width and height to set for the faces of the cube texture.
    static void SetSize(TextureSpecification& spec, unsigned int size)
    {
        spec.SetTextureSize(size, size);
    }
};

/**
 * @brief Get a shared pointer to a cube white texture.
 *
 * @return A cube white texture.
 */
DEFINE_WHITE_TEXTURE(TextureCube)

} // namespace Texturing
} // namespace utils
} // namespace pixc
