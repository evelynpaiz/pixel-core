#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/TextureMaterial.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A material class for sampling depth textures and visualizing linear depth.
 *
 * The `DepthMaterial` class is a subclass of `TextureMaterial` and provides
 * functionality to sample a depth texture, linearize the depth values using the near and far
 * planes of the view frustum, and optionally visualize them as a grayscale texture. It is
 * intended for depth-based effects, shadow mapping, or depth debugging.
 *
 * Near and far plane values are passed as uniforms to the shader for correct linearization.
 *
 * Copying or moving `DepthMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class DepthMaterial : public TextureMaterial
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a depth material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    DepthMaterial(const std::filesystem::path& filePath =
                  ResourcesManager::GeneralPath("pixc/shaders/forward/depth/LinearDepth"))
    : TextureMaterial(filePath)
    {
        // Default to no texture; the depth texture will be assigned externally
        m_Texture = nullptr;
        // Change the name of the texture map
        SetTextureUniformName("u_Material.DepthMap");
    }
    /// @brief Destructor for the depth material.
    ~DepthMaterial() override = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Enable or disable linearization of depth.
    /// @param enable True to linearize depth (for perspective), false to use raw depth (for orthographic).
    void EnableLinearizeDepth(bool enable)
    {
        m_LinearizeDepth = enable;
    }
    
    /// @brief Set the near and far plane values for depth linearization.
    /// @param nearPlane Near clipping plane of the view frustum.
    /// @param farPlane Far clipping plane of the view frustum.
    void SetNearFar(float nearPlane, float farPlane)
    {
        m_NearPlane = nearPlane;
        m_FarPlane = farPlane;
    }
    
private:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        TextureMaterial::SetMaterialProperties();
        m_Shader->SetUint("u_Material.Linearize", (unsigned int)m_LinearizeDepth);
        
        // Linearization properties
        if (!m_LinearizeDepth)
            return;
        m_Shader->SetFloat("u_Material.NearPlane", m_NearPlane);
        m_Shader->SetFloat("u_Material.FarPlane", m_FarPlane);
    }
    
private:
    ///< Whether to linearize depth. Disable for orthographic cameras.
    bool m_LinearizeDepth = true;
    
    ///< Near plane for linearizing depth.
    float m_NearPlane = 0.1f;
    ///< Far plane for linearizing depth.
    float m_FarPlane = 100.0f;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(DepthMaterial);
};

} // namespace pixc
