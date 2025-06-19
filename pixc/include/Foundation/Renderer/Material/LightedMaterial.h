#pragma once

#include "Common/Renderer/Material/Material.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/Light/Light.h"
#include "Common/Renderer/Light/EnvironmentLight.h"

/**
 * A base class for materials that are affected by lighting.
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
            m_LightFlags.ShadowProperties = true;
    }
    
    /// @brief Destructor for the (lighted) material.
    virtual ~LightedMaterial() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Returns the active flags for the lighted material.
    /// @return Light flags.
    LightFlags& GetLightFlags() { return m_LightFlags; }
    
    // Properties
    // ----------------------------------------
    /// @brief Define the light properties linked to the material.
    /// @param lights The set of lights in the scene.
    void DefineLightProperties(LightLibrary& lights)
    {
        m_Shader->Bind();
        m_Shader->SetInt("u_Environment.LightsNumber", lights.GetLightCastersNumber());
        
        // Iterate through each light in the scene
        for (auto& pair : lights)
            DefineLightProperties(pair.second);
    }
    
protected:
    /// @brief Define the light properties linked to the material.
    /// @param light The light object containing the light properties.
    virtual void DefineLightProperties(const std::shared_ptr<BaseLight>& light)
    {
        light->DefineLightProperties(m_Shader, m_LightFlags, m_Slot);
    }
    
    // Lighted color variables
    // ----------------------------------------
protected:
    ///< Flags for shading.
    LightFlags m_LightFlags;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(LightedMaterial);
};
