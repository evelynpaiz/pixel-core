#pragma once

#include "Common/Renderer/Texture/Texture.h"
#include "Common/Renderer/Texture/TextureUtils.h"

/**
 * Provides a static interface for common OpenGL texture operations.
 *
 * This class encapsulates low-level OpenGL functions related to texture management. It offers
 * a simplified way to perform operations like creation, binding, unbinding, and deletion of texture
 * objects.
 *
 * @note `OpenGLTexture` is a non-instantiable utility class. All its members are static, and
 * it's not intended to be subclassed.
 */
class OpenGLTexture
{
public:
    // Texture Creator
    // ----------------------------------------
    void GLCreate();
    // Texture Destructor
    // ----------------------------------------
    void GLRelease();
    
    // Texture Usage
    // ----------------------------------------
    void GLBind(TextureType type) const;
    void GLBindToTextureUnit(TextureType type, uint32_t slot) const;
    void GLUnbind(TextureType type)const;
    
    // Getter(s)
    // ----------------------------------------
    static uint32_t GLGetTextureID(const std::shared_ptr<Texture>& texture);
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class OpenGLFrameBuffer;
    
    // Disable the creation of this resource
    // ----------------------------------------
protected:
    OpenGLTexture();
    
    // Texture variables
    // ----------------------------------------
protected:
    ///< ID of the texture.
    uint32_t m_ID = 0;
};

// Destructor
// ----------------------------------------
#define DEFINE_OPENGL_TEXTURE_DESTRUCTOR(TextureType)\
    /** @brief Destructor to release the OpenGL texture resource. */\
    ~OpenGL##TextureType() override { ReleaseTexture(); }

// Usage
// ----------------------------------------
#define DEFINE_OPENGL_TEXTURE_BINDING_METHODS()\
    /** @brief Binds the texture to the active texture unit. */\
    void Bind() const override\
    {\
        GLBind(m_Spec.Type);\
    }\
    /** @brief Binds the texture to a specific texture unit. */\
    /** @param slot The index of the texture unit to bind to. */\
    void BindToTextureUnit(uint32_t slot) const override\
    {\
        GLBindToTextureUnit(m_Spec.Type, slot);\
    }\
    /** @brief Unbinds the texture from the active texture unit. */\
    void Unbind() const override\
    {\
        GLUnbind(m_Spec.Type);\
    }

// Creation & Release
// ----------------------------------------
#define DEFINE_OPENGL_TEXTURE_RESOURCE_METHODS()\
    /** @brief Releases the underlying OpenGL texture resource. */\
    void ReleaseTexture() override\
    {\
        if (this)\
            GLRelease();\
    }\
    /** @brief Creates the OpenGL texture. */\
    void CreateTexture(const void* data) override;
