#pragma once

#include "Foundation/Core/Resources.h"

#include "Foundation/Renderer/Material/TextureMaterial.h"
#include "Foundation/Renderer/Material/Properties/ColorProperty.h"


/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A material class that combines simple color and texture-based shading.
 *
 * The `UnlitMaterial` class is a subclass of `Material` and provides a basic material
 * definition for shading 3D models with a combination of a simple color and a texture map. It
 * uses a shader specified by the given file path to apply the material properties to the model.
 *
 * Inherit from this class when creating materials that have both an albedo color and a texture map.
 *
 * Copying or moving `UnlitMaterial` objects is disabled to ensure single ownership
 * and prevent unintended duplication of material resources.
 */
class UnlitMaterial : public TextureMaterial, public ColorProperty
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a unlit material object with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    UnlitMaterial(const std::filesystem::path& filePath =
                  ResourcesManager::GeneralPath("pixc/shaders/forward/unlit/Unlit"))
    : TextureMaterial(filePath), ColorProperty()
    {
        m_Texture = pixc::utils::textures::WhiteTexture2D();
    }
    /// @brief Destructor for the unlit material.
    ~UnlitMaterial() override = default;
    
private:
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties into the uniforms of the shader program.
    void SetMaterialProperties() override
    {
        TextureMaterial::SetMaterialProperties();
        ColorProperty::SetProperties(m_Shader, "u_Material.Color");
    }
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(UnlitMaterial);
};

} // namespace pixc
