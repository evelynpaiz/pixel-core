#pragma once

#include "Foundation/Core/Library.h"

#include <type_traits>
#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Enumeration of data types.
 *
 * The `DataType` enumeration represents different data types that can be used for data attributes.
 * It includes boolean, integer, floating-point, vectors (2D, 3D, 4D), and matrices (2x2, 3x3, 4x4) types.
 */
enum class DataType
{
    None,
    Uint, Int, Float,
    Vec2, Vec3, Vec4,
    Mat2, Mat3, Mat4
};


/**
 * @namespace utils::data
 * @brief Utility functions and data structures for data processing and handling.
 */
namespace utils { namespace data {

/**
 * Gets the `DataType` enumeration value corresponding to a given type `T`.
 *
 * @tparam T The type for which to retrieve the corresponding `DataType` value.
 *
 * @return The `DataType` enum value that matches the provided type `T`.
 */
template <typename T>
inline constexpr DataType GetDataType()
{
    if constexpr (std::is_same_v<T, unsigned int>)          { return DataType::Uint; }
    else if constexpr (std::is_same_v<T, int>)              { return DataType::Int; }
    else if constexpr (std::is_same_v<T, float>)            { return DataType::Float; }
    
    else if constexpr (std::is_same_v<T, glm::vec2>)        { return DataType::Vec2; }
    else if constexpr (std::is_same_v<T, glm::vec3>)        { return DataType::Vec3; }
    else if constexpr (std::is_same_v<T, glm::vec4>)        { return DataType::Vec4; }
    
    else if constexpr (std::is_same_v<T, glm::mat2>)        { return DataType::Mat2; }
    else if constexpr (std::is_same_v<T, glm::mat3>)        { return DataType::Mat3; }
    else if constexpr (std::is_same_v<T, glm::mat4>)        { return DataType::Mat4; }
    else
    {
        PIXEL_CORE_ASSERT(false, "Unknown data type!");
        return DataType::None;
    }
}

/**
 * Get the size (in bytes) of the data depending on its type.
 *
 * @param dataType The data type.
 *
 * @return The size of the data (in bytes).
 */
inline uint32_t GetDataSize(DataType dataType)
{
    switch (dataType)
    {
        case DataType::None:  return 0;
        case DataType::Uint:  return 4;
        case DataType::Int:   return 4;
        case DataType::Float: return 4;
        case DataType::Vec2:  return 4 * 2;
        case DataType::Vec3:  return 4 * 3;
        case DataType::Vec4:  return 4 * 4;
        case DataType::Mat2:  return 4 * 2 * 2;
        case DataType::Mat3:  return 4 * 3 * 3;
        case DataType::Mat4:  return 4 * 4 * 4;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown data type!");
    return 0;
}

/**
 * Get the component count of a data type.
 *
 * @param dataType The data type.
 *
 * @return The number of components that the data has.
 */
inline uint32_t GetComponentCount(DataType dataType)
{
    switch (dataType)
    {
        case DataType::None:  return 0;
            
        case DataType::Uint:  return 1;
        case DataType::Int:   return 1;
        case DataType::Float: return 1;
            
        case DataType::Vec2:  return 2;
        case DataType::Vec3:  return 3;
        case DataType::Vec4:  return 4;
            
        case DataType::Mat2:  return 2;
        case DataType::Mat3:  return 3;
        case DataType::Mat4:  return 4;
    }
    
    PIXEL_CORE_ASSERT(false, "Unknown data type!");
    return 0;
}

} // namespace data
} // namespace utils

/**
 * Represents a generic data element within a data layout.
 *
 * The `DataElement` struct holds information about a single data element,
 * including its raw data pointer, data type, size, offset within a larger buffer, and a
 * flag indicating whether it should be normalized (relevant for vertex attributes).
 */
struct DataElement
{
    // Data element variables
    // ----------------------------------------
    ///< Pointer to the raw data.
    void* Data = nullptr;
    ///< Data type of the element (e.g., float, vec3, mat4).
    DataType Type;
    ///< Size of the data in bytes.
    uint32_t Size;
    
    ///< Offset (in bytes) of this element within a larger data layout or buffer.
    size_t Offset = 0;
    ///< Specifies if the data should be normalized.
    bool Normalized = false;
    
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Create a data element.
    DataElement() = default;
    /// @brief Creates a data element with a specific data type.
    /// @param type Data type of the element.
    DataElement(DataType type)
    : Type(type), Size(utils::data::GetDataSize(type))
    {}
    /// @brief Delete the data element.
    virtual ~DataElement()
    {
        if (!Data)
            return;
        
        // Delete the data if necessary
        ::operator delete(Data);
        Data = nullptr;
    }
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Equality operator for comparing two data elements.
    /// @param other Another element to compare against.
    /// @return True if both elements match.
    inline bool operator==(const DataElement& other) const
    {
        return Data == other.Data &&
               Type == other.Type &&
               Size == other.Size &&
               Offset == other.Offset &&
               Normalized == other.Normalized;
    }
};

/**
 * A generic data layout template for organizing data elements.
 *
 * The `DataLayout` class provides a way to define the structure and organization of a
 * collection of data elements. It automatically calculates the offset of each element within the
 * layout and provides the total stride (size in bytes) of the layout.
 *
 * @tparam Element The type of data element to store in the layout. Must be `DataElement`
 * or a type derived from it.
 */
template<typename Element=DataElement>
class DataLayout : public Library<Element>
{
private:
    // Static assertion to enforce the type requirement
    static_assert(std::is_base_of_v<DataElement, Element>,
                  "Element type in DataLayout must be DataElement or derived from it.");
public:
    // Constructor/ Destructor
    // ----------------------------------------
    /// @brief Creates an empty data layout.
    /// @param name  An optional name for the elements contained in the layout (default: "Data element").
    DataLayout(const std::string& name = "Data element") : Library<Element>(name) {}
    /// @brief Creates a data layout from an initializer list of elements.
    /// @param elements A list containing the elements to be added to the layout.
    /// @param name  An optional name for the elements contained in the layout (default: "Data element").
    DataLayout(const std::initializer_list<std::pair<std::string, Element>>& elements,
               const std::string& name = "Data element")
    : Library<Element>(name)
    {
        for (const auto& [name, value] : elements)
            Add(name, value);
    }
    /// @brief Delete the defined layout.
    ~DataLayout() override = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Gets the order in which elements were added to the layout.
    /// @return Gets the order in which elements were added to the layout.
    const std::vector<std::string>& GetBufferOrder() const { return m_Order; }
    
    /// @brief Gets the total size (stride) of the data layout in bytes.
    /// @return The stride value.
    uint32_t GetStride() const { return m_Stride; }
    
    // Add
    // ----------------------------------------
    /// @brief Adds a data element to the layout.
    /// @param name The name to associate with the element.
    /// @param element The data element to add.
    /// @note If an element with the same name already exists in the library, an assertion failure
    /// will occur.
    void Add(const std::string& name,
             const Element& element) override
    {
        Library<Element>::Add(name, element);
        m_Order.push_back(name);
        CalculateOffsetAndStride();
    }
    
private:
    // Calculators
    // ----------------------------------------
    /// @brief Calculates the byte offset of each element within the layout and the total stride of the layout.
    void CalculateOffsetAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (const auto& name : m_Order)
        {
            DataElement &element = static_cast<DataElement&>(this->m_Objects[name]);
            
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }
    
    // Data layout variables
    // ----------------------------------------
private:
    ///< Total size (stride) of the data layout in bytes.
    uint32_t m_Stride = 0;
    
    ///< Insertion order of the elements.
    std::vector<std::string> m_Order;
};

} // namespace pixc

namespace std {

/**
 * @brief Hash function specialization for `DataElement`.
 *
 * @note: Allows `MetalRendererDescriptor` to be used as a key in unordered_map.
 */
template<>
struct hash<pixc::DataElement>
{
    /// @brief Generates a hash for a given `DataElement`.
    /// @param key The descriptor to hash.
    /// @return A combined hash of its data properties.
    size_t operator()(const pixc::DataElement& element) const
    {
        return std::hash<int>()(static_cast<int>(element.Type)) ^
               (std::hash<uint32_t>()(element.Size) << 1) ^
               (std::hash<size_t>()(element.Offset) << 2) ^
               (std::hash<bool>()(element.Normalized) << 3);
    }
};

} // namespace std
