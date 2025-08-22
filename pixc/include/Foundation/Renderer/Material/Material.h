#pragma once

#include "Foundation/Core/Library.h"

#include "Foundation/Renderer/Material/MaterialProperty.h"
#include "Foundation/Renderer/Shader/Shader.h"
#include "Foundation/Renderer/Texture/Texture.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Base class representing a material used for rendering.
 *
 * The `Material` class provides a base interface for defining materials used in rendering.
 * It encapsulates functionality for binding and unbinding the material, as well as setting
 * material-specific properties. Derived classes are responsible for implementing the
 * `SetMaterialProperties()` method to define the material's specific properties.
 *
 * Copying or moving `Material` objects is disabled to ensure single ownership and prevent
 * unintended duplication of material resources.
 */
class Material
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate a material with the specified shader file path.
    /// @param filePath The file path to the shader used by the material.
    Material(const std::filesystem::path& filePath)
    {
        // Define the shader for the material
        std::string name = filePath.stem().string();
        auto shader = s_ShaderLibrary.Exists(name) ?
            s_ShaderLibrary.Get(name) : s_ShaderLibrary.Load(name, filePath);
        m_Shader = shader;
    }
    /// @brief Destructor for the material.
    virtual ~Material() = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Bind the material's associated shader and sets material properties.
    virtual void Bind()
    {
        m_Shader->Bind();
        SetMaterialProperties();
    }
    /// @brief Unbinds the material's associated shader.
    virtual void Unbind()
    {
        m_Shader -> Unbind();
        m_Slot = 0;
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Return the shader associated with the material.
    /// @return shader program.
    std::shared_ptr<Shader> GetShader() { return m_Shader; }
    
    /// @brief Returns the active flags for the material.
    /// @return Shading flags.
    MaterialProperty GetMaterialProperties() { return m_Properties; }
    /// @brief Checks whether a specific `MaterialProperty` flag is set in the current properties.
    /// @param flag The `MaterialProperty` flag to check for.
    /// @return `true` if the `flag` is set in `m_Properties`; otherwise, `false`.
    bool HasProperty(MaterialProperty flag)
    {
        return (m_Properties & flag) != MaterialProperty::None;
    }
    
    // Properties
    // ----------------------------------------
    /// @brief Set the material properties.
    virtual void SetMaterialProperties()
    {}
    
    // Material variables
    // ----------------------------------------
protected:
    ///< The shader used for shading the specific material.
    std::shared_ptr<Shader> m_Shader;
    
    ///< Properties of the material used for shading.
    MaterialProperty m_Properties = MaterialProperty::None;
    ///< Texture unit index.
    unsigned int m_Slot = 0;
    
    ///< Library containing all shader that have been loaded.
    static inline ShaderLibrary s_ShaderLibrary;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Material);
};

/**
 * A library for managing materials used in rendering.
 *
 * The `MaterialLibrary` class provides functionality to add, load, retrieve, and check
 * for the existence of materials within the library. Each material is associated with
 * a unique name.
 */
class MaterialLibrary : public Library<std::shared_ptr<Material>>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new material library.
    MaterialLibrary() : Library("Material") {}
    
    // Create
    // ----------------------------------------
    /// @brief Loads a material and adds it to the library.
    /// @tparam Type The type of material to create.
    /// @tparam Args The types of arguments to forward to the material constructor.
    /// @param name The name to associate with the loaded material.
    /// @param args The arguments to forward to the material constructor.
    /// @return The material created.
    template<typename Type, typename... Args>
    std::shared_ptr<Type> Create(const std::string& name, Args&&... args)
    {
        auto material = std::make_shared<Type>(std::forward<Args>(args)...);
        
        std::string message = GetTypeName() + " '" + name + "' is not of the specified type!";
        PIXEL_CORE_ASSERT(std::dynamic_pointer_cast<Material>(material), message);
        
        Add(name, material);
        return material;
    }
};

} // namespace pixc
