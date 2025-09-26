#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Shader/Shader.h"
#include "Foundation/Renderer/Material/Material.h"
#include "Foundation/Renderer/Buffer/FrameBuffer.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

#include "Foundation/Renderer/Light/LightProperty.h"
#include "Foundation/Renderer/Light/Shadow.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Abstract base class for all light sources in the engine.
 *
 * The `Light`class defines the minimal interface and common data shared by all light types, including
 * both shadow-casting and non-shadow-casting lights (e.g., environment lighting).
 *
 * Copying or moving `Light` objects is disabled to ensure single ownership and prevent unintended
 * duplication of light resources.
 */
class Light
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the base light.
    virtual ~Light() = default;
    
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
    /// @param properties The flags indicating which light properties should be defined.
    virtual void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                                       LightProperty properties) = 0;
    
    /// @brief Check whether a set of `LightProperty` flags contains a specific flag.
    /// @param value The combined set of flags.
    /// @param flag The flag to check.
    /// @return `true` if `flag` is set in `value`; `false` otherwise.
    static bool HasProperty(LightProperty value, LightProperty flag)
    {
        return (value & flag) != LightProperty::None;
    }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a base light.
    Light() = default;
    
    // Light variables
    // ----------------------------------------
protected:
    ///< Overall brightness multiplier for the light.
    float m_Intensity = 1.0f;
    ///< Light model (visible in the scene if defined).
    std::shared_ptr<BaseModel> m_Model;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Light);
};

/**
 * @brief Base class for all shadow-casting lights (directional, point, and spot).
 *
 * The `LightCaster` class extends the generic `Light` interface by adding physical light attributes such as
 * color, diffuse strength, specular strength, and optional shadow mapping capabilities.
 *
 * Copying or moving `LightCaster` objects is disabled to ensure single ownership and prevent unintended
 * duplication of light resources.
 */
class LightCaster : public Light
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Destructor for the light type.
    virtual ~LightCaster() = default;
    
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
    uint32_t GetID() const { return m_ID; }
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
        return m_Shadow.FrameBuffer->GetDepthAttachment();
    }
    
    /// @brief Get the camera used for shadow mapping to generate depth maps for shadow calculations.
    /// @return The viewpoint of the light source.
    const std::shared_ptr<Camera>& GetShadowCamera() const { return m_Shadow.Camera; }
    /// @brief Get the framebuffer with the rendered shadow map.
    /// @return The shadow map framebuffer.
    const std::shared_ptr<FrameBuffer>& GetShadowFrameBuffer() const { return m_Shadow.FrameBuffer; }
    
    // Properties
    // ----------------------------------------
    /// @brief Define light properties into the uniforms of the shader program.
    /// @param shader The shader program.
    /// @param properties The flags indicating which light properties should be defined.
    void DefineLightProperties(const std::shared_ptr<Shader>& shader,
                               LightProperty properties) override
    {

        // Define general light properties if specified by the flags
        if (HasProperty(properties, LightProperty::GeneralProperties))
            DefineGeneralProperties(shader);
    
        // Define strength properties for the light
        DefineStrenghtProperties(shader, properties);
        
        // Define transformation properties if specified by the flags
        if (HasProperty(properties, LightProperty::ShadowProperties))
        {
            DefineTranformProperties(shader);
            uint32_t slot = static_cast<uint32_t>(TextureIndex::ShadowMap0) + GetID();
            shader->SetTexture("u_Environment.Lights[" + std::to_string(GetID()) + "].ShadowMap",
                               GetShadowMap(), slot);
        }
    }
    
    // Shadow
    // ----------------------------------------
    /// @brief Initialize the shadow map framebuffer.
    /// @param width The width of the shadow map in pixels.
    /// @param height The height of the shadow map in pixels.
    /// @param format The depth texture format.
    void InitShadowFrameBuffer(int width, int height, TextureFormat format = TextureFormat::DEPTH24)
    {
        m_Shadow.Camera->SetViewportSize(width, height);
        
        FrameBufferSpecification spec;
        spec.SetFrameBufferSize(width, height);
        spec.AttachmentsSpec = {
            { TextureType::TEXTURE2D, format }
        };
        m_Shadow.FrameBuffer = FrameBuffer::Create(spec);
    }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a light source.
    /// @param color The color of the light source.
    LightCaster(const glm::vec4 &vector,
                const glm::vec3 &color = glm::vec3(1.0f))
    : Light(), m_ID(s_IndexCount++), m_Vector(vector), m_Color(color)
    {
        // Define the depth material if it has not been define yet
        auto& library = Renderer::GetMaterialLibrary();
        if (!library.Exists("Depth"))
        {
            library.Create<Material>("Depth", ResourcesManager::GeneralPath("pixc/shaders/depth/DepthMap"));
        }
    }
    
    // Properties
    // ----------------------------------------
    /// @brief Define the general properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineGeneralProperties(const std::shared_ptr<Shader> &shader)
    {
        shader->SetVec3("u_Environment.Lights[" + std::to_string(m_ID) + "].Color", m_Color);
        shader->SetVec4("u_Environment.Lights[" + std::to_string(m_ID) + "].Vector", m_Vector);
    }
    /// @brief Define the strength properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineStrenghtProperties(const std::shared_ptr<Shader> &shader,
                                  LightProperty properties)
    {
        if (HasProperty(properties, LightProperty::DiffuseLighting))
            shader->SetFloat("u_Environment.Lights[" + std::to_string(m_ID) + "].Ld", m_DiffuseStrength);
        if (HasProperty(properties, LightProperty::SpecularLighting))
            shader->SetFloat("u_Environment.Lights[" + std::to_string(m_ID) + "].Ls", m_SpecularStrength);
    }
    /// @brief Define the transformation properties (from the light) into the uniforms of the shader program.
    /// @param shader Shader program to be used.
    void DefineTranformProperties(const std::shared_ptr<Shader> &shader)
    {
        shader->SetMat4("u_Environment.Lights[" + std::to_string(m_ID) + "].Transform",
                        m_Shadow.Camera->GetProjectionMatrix() *
                        m_Shadow.Camera->GetViewMatrix());
    }
    
protected:
    // Light variables
    // ----------------------------------------
    ///< The index id of the light source.
    uint32_t m_ID;
    
    ///< The position of the light if .w is defined as 1.0f, or
    ///< the direction of the light if .w is defined as 0.0f.
    glm::vec4 m_Vector;
    
    ///< The light color.
    glm::vec3 m_Color;
    
    ///< The light intensities.
    float m_DiffuseStrength = 0.6f;
    float m_SpecularStrength = 0.4f;
    
    /// @brief Encapsulates shadow-related data for the light caster.
    struct ShadowMap
    {
        ///< Camera representing the light’s viewpoint for shadow mapping.
        std::shared_ptr<Camera> Camera;
        ///< Framebuffer used to render the shadow map texture.
        std::shared_ptr<FrameBuffer> FrameBuffer;
    };

    ///< Shadow data container.
    ShadowMap m_Shadow;
    
    static inline uint32_t s_IndexCount = 0;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(LightCaster);
};

/**
 * @brief A library for managing lights used in rendering.
 *
 * The `LightLibrary` class provides functionality to add, load, retrieve, and check
 * for the existence of lights within the library. Each light is associated with
 * a unique name.
 */
class LightLibrary : public Library<std::shared_ptr<Light>>
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
             const std::shared_ptr<Light>& light) override
    {
        // Add the light to the library
        Library::Add(name, light);
        
        // Count it as light caster if necessary
        if (std::dynamic_pointer_cast<LightCaster>(light))
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
        PIXEL_CORE_ASSERT(std::dynamic_pointer_cast<Light>(light), message);
        
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

} // namespace pixc
