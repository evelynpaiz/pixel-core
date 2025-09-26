#pragma once

#include "Foundation/Renderer/Light/Light.h"
#include "Foundation/Renderer/Material/Material.h"

#include "Foundation/Renderer/Buffer/FrameBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief A base class for materials that are affected by lighting.
 *
 * The `LightedMaterial` class extends the base `Material` class to provide additional
 * functionality for handling lighting and shadow properties. It allows setting a light
 * source and a framebuffer for shadow mapping, as well as defining shadow properties
 * in the shader program. Derived classes must implement the `SetMaterialProperties()`
 * method to specify the material's specific properties.
 *
 * Copying or moving `LightedMaterial` objects is disabled to ensure single ownership and
 * prevent unintended duplication of material resources.
 */
class LightedMaterial : public Material
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Generate a (lighted) material with the specified shader file path.
    /// @param light The light source to be used for shading.
    /// @param filePath The file path to the shader used by the material.
    LightedMaterial(const std::filesystem::path& filePath)
        : Material(filePath)
    {
        // Get the file name from the path
        std::string filename = filePath.filename().string();
        // Convert the filename to lowercase for case-insensitive comparison
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
        // Check if the filename contains the word "shadow"
        if (filename.find("shadow") != std::string::npos)
            m_LightProperties |= LightProperty::ShadowProperties;
    }
    
    /// @brief Destructor for the (lighted) material.
    virtual ~LightedMaterial() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves the currently active light properties for the material.
    /// @return The combined `LightProperty` flags representing active light properties.
    LightProperty GetLightProperties() { return m_LightProperties; }
    
    // Properties
    // ----------------------------------------
    /// @brief Define the light properties linked to the material.
    /// @param lights The set of lights in the scene.
    void DefineLightProperties(LightLibrary& lights)
    {
        m_Shader->Bind();
        m_Shader->SetInt("u_Environment.LightCount", lights.GetLightCastersNumber());
        
        // Iterate through each light in the scene
        for (auto& pair : lights)
            DefineLightProperties(pair.second);
    }
    
protected:
    /// @brief Define the light properties linked to the material.
    /// @param light The light object containing the light properties.
    virtual void DefineLightProperties(const std::shared_ptr<Light>& light)
    {
        light->DefineLightProperties(m_Shader, m_LightProperties);
    }
    
    // Lighted color variables
    // ----------------------------------------
protected:
    ///< Flags for shading.
    LightProperty m_LightProperties = LightProperty::GeneralProperties |
                                      LightProperty::DiffuseLighting |
                                      LightProperty::SpecularLighting;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(LightedMaterial);
};

} // namespace pixc
