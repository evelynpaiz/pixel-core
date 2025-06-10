#pragma once

#include "Common/Renderer/Shader/Shader.h"
#include "Common/Renderer/Material/Material.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/Light/ShadowCamera.h"
#include "Common/Renderer/Drawable/Model/Model.h"

#include <glm/glm.hpp>

/**
 * @brief Flags representing properties of a lighted object.
 *
 * The `LightFlags` struct defines various flags indicating properties of a lighted object,
 * such as whether general properties, transformation properties, ambient lighting, diffuse
 * lighting, and specular lighting are enabled.
 */
struct LightFlags
{
    bool GeneralProperties = true;          ///< Indicates whether general properties are enabled.
    bool ShadowProperties = false;          ///< Indicates whether shadows properties are enabled.
    
    bool DiffuseLighting = true;            ///< Indicates whether diffuse lighting is enabled.
    bool SpecularLighting = true;           ///< Indicates whether specular lighting is enabled.
    
    bool IsotropicShading = true;           ///< Indicates whether to use normal-based isotropic shading or tangent-based anisotropic shading.
};

/**
 * Base class for light.
 *
 * Copying or moving `BaseLight` objects is disabled to ensure single ownership and prevent unintended
 * duplication of light resources.
 */
class BaseLight
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the base light.
    virtual ~BaseLight() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the light 3D model representing the light.
    /// @return The light 3D model.
    const std::shared_ptr<BaseModel>& GetModel() { return m_Model; }
    
    // Render
    // ----------------------------------------
    /// @brief Renders the 3D model that represents the light source.
    virtual void DrawLight()
    {
        if (m_Model)
            m_Model->DrawModel();
    }
    
    /// @brief Define light properties into the uniforms of the shader program.
    /// @param shader The shader program.
    /// @param flags The flags indicating which light properties should be defined.
    virtual void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                                       const LightFlags& flags,
                                       unsigned int& slot) = 0;
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a base light.
    BaseLight()
    {
        // Define the depth material if it has not been define yet
        auto& library = Renderer::GetMaterialLibrary();
        if (!library.Exists("Depth"))
            library.Create<Material>("Depth", "Resources/shaders/depth/DepthMap");
    }
    
    // Light variables
    // ----------------------------------------
protected:
    ///< Light model (visible in the scene).
    std::shared_ptr<BaseModel> m_Model;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(BaseLight);
};

/**
 * Base class for light sources used in a scene.
 *
 * The `Light` class serves as a base class for defining different types of light sources used in 3D
 * rendering. It provides common functionality for defining and retrieving the color of the light source.
 * Derived classes can override the `DefineLightProperties` method to set additional light
 * properties in the shader, such as position or direction.
 *
 * Copying or moving `Light` objects is disabled to ensure single ownership and prevent unintended
 * duplication of light resources.
 */
class Light : public BaseLight
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the light type.
    virtual ~Light() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the color of the light source.
    /// @param color The color of the light source.
    void SetColor(const glm::vec3 &color) { m_Color = color; }
    
    /// @brief Set the strength of the diffuse component of the light source.
    /// @param s The strength of the diffuse component (a value between 0 and 1).
    void SetDiffuseStrength(float s) { m_DiffuseStrength = s; }
    /// @brief Set the strength of the specular component of the light source.
    /// @param s The strength of the specular component (a value between 0 and 1).
    void SetSpecularStrength(float s) { m_SpecularStrength = s; }
    
    // Getter(s)
    // ----------------------------------------
    ///< @brief Get the ID of the light source.
    ///< @return Light ID number.
    unsigned int GetID() const { return m_ID; }
    /// @brief Get the color of the light source.
    /// @return The color of the light source.
    glm::vec3 GetColor() const { return m_Color; }
    
    /// @brief Get the diffuse strength of the light source.
    /// @return The diffuse strength of the light source.
    float GetDiffuseStrength() const { return m_DiffuseStrength; }
    /// @brief Get the specular strength of the light source.
    /// @return The specular strength of the light source.
    float GetSpecularStrength() const { return m_SpecularStrength; }
    
    /// @brief Get the texture containing the shadow map of the light source.
    /// @return The shadow map.
    const std::shared_ptr<Texture>& GetShadowMap() const
    {
        return m_Framebuffer->GetDepthAttachment();
    }
    
    /// @brief Get the camera used for shadow mapping to generate depth maps for shadow calculations.
    /// @return The viewpoint of the light source.
    const std::shared_ptr<Camera>& GetShadowCamera() const { return m_ShadowCamera; }
    /// @brief Get the framebuffer with the rendered shadow map.
    /// @return The shadow map framebuffer.
    const std::shared_ptr<FrameBuffer>&  GetFramebuffer() const { return m_Framebuffer; }
    
    // Properties
    // ----------------------------------------
    /// @brief Define the general properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineGeneralProperties(const std::shared_ptr<Shader> &shader)
    {
        shader->SetVec3("u_Light[" + std::to_string(m_ID) + "].Color", m_Color);
        shader->SetVec4("u_Light[" + std::to_string(m_ID) + "].Vector", m_Vector);
    }
    /// @brief Define the strength properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineStrenghtProperties(const std::shared_ptr<Shader> &shader,
                                  const LightFlags& flags)
    {
        if (flags.DiffuseLighting)
            shader->SetFloat("u_Light[" + std::to_string(m_ID) + "].Ld", m_DiffuseStrength);
        if (flags.SpecularLighting)
            shader->SetFloat("u_Light[" + std::to_string(m_ID) + "].Ls", m_SpecularStrength);
    }
    /// @brief Define the transformation properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineTranformProperties(const std::shared_ptr<Shader> &shader)
    {
        shader->SetMat4("u_Light[" + std::to_string(m_ID) + "].Transform",
                        m_ShadowCamera->GetProjectionMatrix() *
                        m_ShadowCamera->GetViewMatrix());
    }
    
    /// @brief Define light properties into the uniforms of the shader program.
    /// @param shader The shader program.
    /// @param flags The flags indicating which light properties should be defined.
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               const LightFlags& flags,
                               unsigned int& slot) override
    {
        // Define general light properties if specified by the flags
        if (flags.GeneralProperties)
            DefineGeneralProperties(shader);
        
        // Define strength properties for the light
        DefineStrenghtProperties(shader, flags);
        
        // Define transformation properties if specified by the flags
        if (flags.ShadowProperties)
        {
            DefineTranformProperties(shader);
            shader->SetTexture("u_Light[" + std::to_string(GetID()) + "].ShadowMap",
                               GetShadowMap(), slot++);
        }
    }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a light source.
    /// @param color The color of the light source.
    Light(const glm::vec4 &vector,
          const glm::vec3 &color = glm::vec3(1.0f))
        : BaseLight(), m_ID(s_IndexCount++), m_Vector(vector), m_Color(color)
    {};
    /// @brief Initialize the shadow map framebuffer.
    /// @param width Framebuffer's width.
    /// @param height Framebuffer's height.
    void InitShadowMapBuffer(int width, int height)
    {
        FrameBufferSpecification spec;
        spec.SetFrameBufferSize(width, height);
        spec.AttachmentsSpec = {
            { TextureType::TEXTURE2D, TextureFormat::DEPTH24 }
        };
        m_Framebuffer = FrameBuffer::Create(spec);
    }
    
protected:
    // Light variables
    // ----------------------------------------
    ///< The index id of the light source.
    unsigned int m_ID;
    
    ///< The position of the light if .w is defined as 1.0f, or
    ///< the direction of the light if .w is defined as 0.0f.
    glm::vec4 m_Vector;
    
    ///< The light color.
    glm::vec3 m_Color;
    
    ///< The light intensities.
    float m_DiffuseStrength = 0.6f;
    float m_SpecularStrength = 0.4f;
    
    ///< The light viewpoint (used for rendering shadows).
    std::shared_ptr<Camera> m_ShadowCamera;
    ///< Framebuffer to render into the shadow map.
    std::shared_ptr<FrameBuffer> m_Framebuffer;
    
    static inline unsigned int s_IndexCount = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Light);
};

/**
 * A library for managing lights used in rendering.
 *
 * The `LightLibrary` class provides functionality to add, load, retrieve, and check
 * for the existence of lights within the library. Each light is associated with
 * a unique name.
 */
class LightLibrary : public Library<std::shared_ptr<BaseLight>>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new light library.
    LightLibrary() : Library("Light"), m_Casters(0) {}
    
    // Add/Create
    // ----------------------------------------
    /// @brief Adds an object to the library.
    /// @param name The name to associate with the object.
    /// @param object The object to add.
    /// @note If an object with the same name already exists in the library, an assertion failure
    /// will occur.
    void Add(const std::string& name,
             const std::shared_ptr<BaseLight>& light) override
    {
        // Add the light to the library
        Library::Add(name, light);
        
        // Count it as light caster if necessary
        if (std::dynamic_pointer_cast<Light>(light))
            m_Casters++;
    }
    
    /// @brief Loads a material and adds it to the library.
    /// @tparam Type The type of the light to create.
    /// @tparam Args The types of arguments to forward to the light constructor.
    /// @param name The name to associate with the loaded light.
    /// @param args The arguments to forward to the light constructor.
    /// @return The light created.
    template<typename Type, typename... Args>
    std::shared_ptr<Type> Create(const std::string& name, Args&&... args)
    {
        auto light = std::make_shared<Type>(std::forward<Args>(args)...);
        
        std::string message = GetTypeName() + " '" + name + "' is not of the specified type!";
        CORE_ASSERT(std::dynamic_pointer_cast<BaseLight>(light), message);
        
        Add(name, light);
        return light;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the number of direct lights (light casters)
    /// @return The counter of lights.
    int GetLightCastersNumber() const { return m_Casters; }
    
    // Library variables
    // ----------------------------------------
private:
    ///< Number of light casters in the library.
    int m_Casters;
};
