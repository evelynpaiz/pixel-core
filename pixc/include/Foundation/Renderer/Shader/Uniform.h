#pragma once

#include "Foundation/Core/Library.h"

#include "Foundation/Renderer/Buffer/Data.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Defines the types of shaders available.
 */
enum class ShaderType
{
    NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
};

/**
 * @brief Represents a texture element used in a shader.
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
    void AddShaderType(ShaderType type)
    {
        // Add the shader type only if it's not already present
        if (std::find(ShaderTypes.begin(), ShaderTypes.end(), type) == ShaderTypes.end())
            ShaderTypes.push_back(type);
    }
};

/**
 * @brief Represents a single variable inside a uniform block.
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
    UniformElement(DataType type) : DataElement(type)
    {}
};

/**
 * @brief Represents a node within a uniform hierarchy.
 *
 * A `UniformMember` can act either as a container for a single `UniformElement` or as a parent
 * that groups together multiple named child members. This makes it possible to represent simple uniform
 * variables, as well as more complex data structures such as nested structs or arrays of structs. By
 * chaining together multiple `UniformMember` objects, a complete uniform block can be modeled as a
 * hierarchical tree where the leaves hold concrete `UniformElement` values.
 */
class UniformMember
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    ///@brief Create a node.
    UniformMember() = default;
    ///@brief Delete the node.
    ~UniformMember() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the uniform element for this node (used if it is a leaf node).
    /// @param element The `UniformElement` to store in this node.
    void SetElement(const UniformElement& element)
    {
        PIXEL_CORE_ASSERT(m_Members.IsEmpty(), "Trying to set a uniform element in a struct node!");
        m_Element = element;
    }
    /// @brief Sets the uniform element for this node.
    /// @param name The name of the member to add.
    /// @param element The `UniformElement` to store in this node.
    void SetElement(const std::string& name,
                    const UniformElement& element)
    {
        // Empty name, then set element on the current node
        if(name.empty())
        {
            SetElement(element);
            return;
        }
    
        // Start from the current node
        UniformMember* current = this;
        std::string remaining = name;

        // Traverse hierarchy until reaching the leaf
        while (!remaining.empty())
        {
            auto [head, tail] = utils::SplitString(remaining);

            // Create intermediate node if it doesn't exist
            if (!current->m_Members.Exists(head))
                current->AddMember(head, UniformMember{});

            // Move deeper
            current = &current->m_Members.Get(head);
            remaining = tail;
        }

        // Set the element on the leaf node
        current->SetElement(element);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves a uniform element by a hierarchical name.
    /// @param name The hierarchical name of the uniform element to retrieve.
    /// @return Reference to the `UniformElement` corresponding to the name.
    UniformElement& GetElement(const std::string& name)
    {
        // Leaf node requested
        if(name.empty())
        {
            PIXEL_CORE_ASSERT(m_Element.has_value(), "Leaf element does not exist!");
            return *m_Element;
        }
        
        // Element of an internal node requested
        auto [head, tail] = utils::SplitString(name);
        return m_Members.Get(head).GetElement(tail);
    }
    
    /// @brief Checks whether a uniform element exists in this node.
    /// @param name The hierarchical name of the element to check.
    /// @return True if the element exists, false otherwise.
    bool Exists(const std::string& name) const
    {
        // No name defined for search
        if (name.empty())
            return false;

        // Element of an internal node requested, split the name
        auto [head, tail] = utils::SplitString(name);
        
        // Check if the head exists first
        if (!m_Members.Exists(head))
            return false;
        // There are no other members to check?
        if (tail.empty())
            return true;
        
        // Check the internal members
        return m_Members.Get(head).Exists(tail);
    }
    
    /// @brief Gets the order in which elements were added to the node.
    /// @return The order in which elements were added.
    const std::vector<std::string>& GetMembersOrder() const { return m_Order; }
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class Uniform;
    friend class UniformLibrary;
    
private:
    // Setter(s)
    // ----------------------------------------
    /// @brief Adds a child member to this node.
    /// @param name The name of the member to add.
    /// @param member The `UniformNode` representing the member.
    void AddMember(const std::string& name,
                   const UniformMember& member)
    {
        m_Members.Add(name, member);
        m_Order.push_back(name);
    }
    
private:
    // Uniform element variables
    // ----------------------------------------
    ///< Leaf node (single value)
    std::optional<UniformElement> m_Element;
    
    ///< Branch nodes (members with names)
    Library<UniformMember> m_Members;
    ///< Insertion order of the elements.
    std::vector<std::string> m_Order;
};

/**
 * @brief Represents a uniform block used in a shader.
 *
 * The `Uniform` class models the structure and layout of uniform data. It owns a root `UniformMember`
 * node that defines the hierarchical organization of the uniform block, allowing access to individual
 * `UniformElement` values through the hierarchy.
 *
 * In addition to the structure, `Uniform` keeps track of metadata such as the total stride (size in bytes), the
 * binding index used in the shader, and the shader stages that utilize the uniform block. It also manages a pointer to
 * the raw buffer containing the uniform data.
 */
class Uniform
{
public:
    // Constructor/ Destructor
    // ----------------------------------------
    /// @brief Creates an empty uniform layout.
    Uniform() = default;
    /// @brief Delete the defined layout.
    ~Uniform() = default;
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the uniform element in the root node.
    /// @param name The name of the member to add.
    /// @param element The `UniformElement` to store in this node.
    void SetElement(const std::string& name,
                    const UniformElement& element)
    {
        m_Root.SetElement(name, element);
        m_Stride = (uint32_t)CalculateOffsets(m_Root, 0);
    }
    
    /// @brief Sets the binding index for this uniform layout in the shader.
    /// @param index The binding index.
    void SetIndex(const int32_t index) { m_Index = index; }
    
    /// @brief Sets the shader type (vertex, fragment, etc.) that uses this uniform layout.
    /// @param type The shader type.
    void AddShaderType(ShaderType type)
    {
        // Add the shader type only if it's not already present
        if (std::find(m_ShaderTypes.begin(), m_ShaderTypes.end(), type) == m_ShaderTypes.end())
            m_ShaderTypes.push_back(type);
    }
    
    /// @brief Sets the pointer to the raw data buffer for this uniform layout.
    /// @return A pointer to the data buffer, or `nullptr` if not yet allocated.
    void SetBufferOfData(void* buffer) { m_Buffer = buffer; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves a uniform element by a hierarchical name.
    /// @param name The hierarchical name of the uniform element to retrieve.
    /// @return Reference to the `UniformElement` corresponding to the name.
    UniformElement& GetElement(const std::string& name) { return m_Root.GetElement(name); }
    
    /// @brief Gets the total size (stride) of the data layout in bytes.
    /// @return The stride value.
    uint32_t GetStride() const { return m_Stride; }
    
    /// @brief Gets the binding index for this uniform layout.
    /// @return The binding index.
    int32_t GetIndex() const { return m_Index; }
    
    /// @brief Gets a pointer to the raw data buffer for this uniform layout.
    /// @return A pointer to the data buffer, or `nullptr` if not yet allocated.
    void* GetBufferOfData() const { return m_Buffer; }
    
    /// @brief Gets the shader type (vertex, fragment, etc.) associated with this uniform layout.
    /// @return The shader type.
    const std::vector<ShaderType>& GetShaderType() const { return m_ShaderTypes; }
    
    /// @brief Checks if a uniform element with the given hierarchical name exists.
    /// @param name The hierarchical name to check.
    /// @return True if the element exists, otherwise false.
    bool Exists(const std::string& name) const
    {
        return m_Root.Exists(name);
    }
    
private:
    
    // Calculators
    // ----------------------------------------
    /// @brief Recursively calculates the byte offset of each uniform element in a hierarchical layout.
    /// @param node The uniform member node whose elements' offsets will be calculated.
    /// @param startOffset The starting byte offset for this node within the layout.
    /// @return The byte offset immediately after the last element in this node, including all nested members.
    uint32_t CalculateOffsets(UniformMember& node, uint32_t startOffset)
    {
        uint32_t offset = startOffset;

        // If leaf node
        if (node.m_Element.has_value())
        {
            node.m_Element->Offset = offset;
            offset += node.m_Element->Size;
        }

        // If internal node
        for (const auto& name : node.GetMembersOrder())
        {
            auto& child = node.m_Members.Get(name);
            offset = CalculateOffsets(child, offset);
        }

        return offset; // return end offset
    }
    
    // Uniform variables
    // ----------------------------------------
private:
    ///< Total size (stride) of the data layout in bytes.
    uint32_t m_Stride = 0;
    
    ///< Binding index for this uniform layout in the shader.
    int32_t m_Index = -1;
    ///< Pointer to the raw data buffer for the uniform data.
    void* m_Buffer = nullptr;
    
    ///< Root node of the uniform hierarchy.
    UniformMember m_Root;
    
    ///< Shader type (vertex, fragment, etc.) that uses this uniform layout.
    std::vector<ShaderType> m_ShaderTypes;
};

/**
 * @brief A library for managing uniform layouts.
 *
 * This class extends `Library<UniformLayout>` to provide a specialized container
 * for organizing and accessing uniform layouts associated with shaders.
 *
 * It allows adding, retrieving, and checking the existence of uniform elements,
 * grouping them by uniform block names and member names.
 */
class UniformLibrary : public Library<Uniform>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new shader library.
    UniformLibrary() : Library("Uniform blocks") {}
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
        // Makes sure the object has not been added before
        std::string message = GetTypeName() + " '" +
        utils::MergeStrings(group, member) + "' already exists!";
        PIXEL_CORE_ASSERT(!Exists(group, member), message);
        
        // Ensure group layout exists
        if (!Library::Exists(group))
            m_Objects[group] = Uniform();
        
        m_Objects[group].SetElement(member, object);
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
        PIXEL_CORE_ASSERT(Exists(group, member), message);
        
        return m_Objects[group].GetElement(member);
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

} // namespace pixc
