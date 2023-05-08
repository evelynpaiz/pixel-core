#pragma once

/**
 * Represents an image used to add details to rendered geometry.
 *
 * The `Texture` class provides functionality to load and bind textures that can be applied to
 * geometry during rendering. Textures can be loaded from file paths and bound to specific texture
 * slots for use in a `Shader`. The class also supports unbinding of textures.
 *
 * Copying or moving `Texture` objects is disabled to ensure single ownership and prevent
 * unintended texture duplication.
 */
class Texture {
public:
    // Constructor(s)/Destructor
    Texture(const std::filesystem::path& filePath, bool flip = true);
    ~Texture();
    // Usage
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
        
private:
    // Loading
    void LoadFromFile(const std::filesystem::path& filePath);
    void Generate2DTexture(const void *data);
    
// Remove the possibility of copying or moving this resource
public:
    // Constructors
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    // Operators
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;
    
private:
    ///< Path to the file.
    std::filesystem::path m_FilePath;
    ///< Texture flipping.
    bool m_Flip;
    
    ///< ID of the texture.
    unsigned int m_ID = 0;

    ///< Size (width and height).
    int m_Width = 0, m_Height = 0;
    ///< Number of channels.
    int m_Channels = 0;
    
    ///< Format.
    unsigned int m_InternalFormat = 0;
    unsigned int m_DataFormat = 0;
};
