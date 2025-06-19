#pragma once

#include "Common/Core/Library.h"
#include "Common/Renderer/Buffer/Data.h"

#include <glm/glm.hpp>

/**
 * Defines the types of shaders available.
 */
enum class ShaderType
{
    NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
};

/**
 * Represents a texture element used in a shader.
 */
struct TextureElement
{
    ///< Binding index for the texture in the shader.
    int32_t Index = -1;
    
    ///< Shader types (vertex, fragment, etc.) that use the texture.
    std::vector<ShaderType> ShaderTypes;
    
    // Constructor(s)/Destructor
    // ----------------------------------------
    ///@brief Create a texture element.
    TextureElement() = default;
    ///@brief Create a texture element with a defined index.
    TextureElement(const int32_t index) : Index(index) {}
    ///@brief Delete the texture element.
    ~TextureElement() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the shader type (vertex, fragment, etc.) that use the texture.
    /// @param type The shader type.
    void SetShaderType(ShaderType type)
    {
        // Add the shader type only if it's not already present
        if (std::find(ShaderTypes.begin(), ShaderTypes.end(), type) == ShaderTypes.end())
            ShaderTypes.push_back(type);
    }
};

/**
 * Represents a uniform element within a uniform layout.
 *
 * This struct extends `DataElement` to include information specific to uniform variables,
 * such as their location (offset) within a uniform buffer and a flag indicating whether the data
 * needs to be updated.
 */
struct UniformElement : public DataElement
{
    // Uniform element variables
    // ----------------------------------------
    ///< Offset (in bytes) of this uniform within a uniform buffer.
    int32_t Location = -1;
    ///< Flag indicating whether the uniform data needs to be updated.
    bool Update = true;
    
    // Constructor(s)/Destructor
    // ----------------------------------------
    ///@brief Create a uniform element.
    UniformElement() : DataElement() {}
    /// @brief Creates a uniform element with a specific data type.
    /// @param type Data type of the uniform element.
    UniformElement(DataType type)
        : DataElement(type)
    {}
};

/**
 * Represents a layout for uniform data used in a shader.
 *
 * This class specializes `DataLayout` to manage a collection of `UniformElement`
 * objects. It provides functionality for storing the uniform data, calculating offsets, determining
 * the binding index, and identifying the shader type (vertex, fragment, etc.) associated with
 * the uniform layout.
 */
class UniformLayout : public DataLayout<UniformElement>
{
public:
    // Constructor/ Destructor
    // ----------------------------------------
    /// @brief Creates an empty uniform layout. 
    UniformLayout() : DataLayout("Uniform element") {}
    /// @brief Creates a buffer layout from an initializer list of uniform elements.
    /// @param elements A list containing the elements to be added to the layout.
    UniformLayout(const std::initializer_list<std::pair<std::string, UniformElement>>& elements)
            : DataLayout(elements, "Uniform element")
    {}
    /// @brief Delete the defined layout.
    ~UniformLayout() override = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the binding index for this uniform layout in the shader.
    /// @param index The binding index.
    void SetIndex(const int32_t index) { m_Index = index; }
    
    /// @brief Sets the shader type (vertex, fragment, etc.) that uses this uniform layout.
    /// @param type The shader type.
    void SetShaderType(ShaderType type)
    {
        // Add the shader type only if it's not already present
        if (std::find(m_ShaderTypes.begin(), m_ShaderTypes.end(), type) == m_ShaderTypes.end())
            m_ShaderTypes.push_back(type);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Gets the binding index for this uniform layout.
    /// @return The binding index.
    int32_t GetIndex() const { return m_Index; }
    
    /// @brief Gets a pointer to the raw data buffer for this uniform layout.
    /// @return A pointer to the data buffer, or `nullptr` if not yet allocated.
    void* GetBufferOfData() const { return m_Buffer; }
    
    /// @brief Gets the shader type (vertex, fragment, etc.) associated with this uniform layout.
    /// @return The shader type.
    const std::vector<ShaderType>& GetShaderType() const { return m_ShaderTypes; }
    
    // Uniform layout variables
    // ----------------------------------------
private:
    ///< Pointer to the raw data buffer for the uniform data.
    void* m_Buffer = nullptr;
    ///< Binding index for this uniform layout in the shader.
    int32_t m_Index = -1;
    
    ///< Shader type (vertex, fragment, etc.) that uses this uniform layout.
    std::vector<ShaderType> m_ShaderTypes;
};

/**
 * A library for managing uniform layouts in a shader.
 *
 * This class extends `Library<UniformLayout>` to provide a specialized container
 * for organizing and accessing uniform layouts associated with a shader. It allows you to
 * add, retrieve, and check the existence of uniform elements, grouping them by uniform
 * block names.
 */
class UniformLibrary : public Library<UniformLayout>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new shader library.
    UniformLibrary() : Library("Uniform element") {}
    /// @brief Delete the defined layout.
    ~UniformLibrary() override = default;
    
    // Add
    // ----------------------------------------
    /// @brief Adds an object to the library.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @param object The object to add.
    /// @note If an object with the same group and object names already exists, an assertion failure
    /// will occur.
    virtual void Add(const std::string& group,
                     const std::string& member,
                     const UniformElement& object)
    {
        std::string message = GetTypeName() + " '" +
            utils::MergeStrings(group, member) + "' already exists!";
        CORE_ASSERT(!Exists(group, member), message);

        m_Objects[group].Add(member, object);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves an object from the library by its group and object names.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return The retrieved object.
    /// @note If the object does not exist, an assertion failure will occur.
    UniformElement& Get(const std::string& group,
                        const std::string& member)
    {
        std::string message = GetTypeName() + " '" +
            utils::MergeStrings(group, member) + "' not found!";
        CORE_ASSERT(Exists(group, member), message);
        
        return m_Objects[group].Get(member);
    }

    /// @brief Retrieves an object from the library by its group and object names (const version).
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return The retrieved object.
    /// @note If the object does not exist, an assertion failure will occur.
    const UniformElement& Get(const std::string& group,
                              const std::string& member) const
    {
        std::string message = GetTypeName() + " '" +
            utils::MergeStrings(group, member) + "' not found!";
        CORE_ASSERT(Exists(group, member), message);
        return m_Objects.at(group).Get(member);
    }
    
    /// @brief Checks if an object with given group and object names exists in the library.
    /// @param group The name of the group to associate with the object.
    /// @param member The name of the member of a group.
    /// @return True if the object exists, otherwise false.
    bool Exists(const std::string& group,
                const std::string& member) const
    {
        auto groupIt = m_Objects.find(group);
        if (groupIt == m_Objects.end())
            return false;
        return groupIt->second.Exists(member);
    }
};
