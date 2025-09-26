#pragma once

#include "Foundation/Renderer/Buffer/Data.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Specialized data layout for vertex buffer attributes.
 *
 * This class derives from `DataLayout<DataElement>` and is specifically
 * designed for defining the arrangement and format of vertex attributes within a
 * vertex buffer.
 *
 * It inherits the functionality of `DataLayout` for managing the elements and
 * calculating offsets and stride.
 *
 * @note `BufferLayout` objects are typically used in conjunction with a
 *       `VertexBuffer` (or a similar mechanism) to define how vertex data
 *       is structured in GPU memory.
 */
class BufferLayout : public DataLayout<DataElement>
{
public:
    // Constructor/ Destructor
    // ----------------------------------------
    /// @brief Creates an empty buffer layout.
    BufferLayout() : DataLayout("Buffer element") {}
    /// @brief Creates a buffer layout from an initializer list of buffer elements.
    /// @param elements A list containing the elements to be added to the layout.
    BufferLayout(const std::initializer_list<std::pair<std::string, DataElement>>& elements)
        : DataLayout(elements, "Buffer element")
    {}
    /// @brief Delete the defined layout.
    ~BufferLayout() override = default;
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Equality operator for comparing two buffer layouts.
    /// @param other Another buffer layout to compare against.
    /// @return True if both layouts match.
    inline bool operator==(const pixc::BufferLayout& other) const
    {
        // Compare names and order or names
        if (GetBufferOrder() != other.GetBufferOrder())
            return false;

        // Check each element and compare between each one
        for (const auto& name : GetBufferOrder())
        {
            if (!(Get(name) == other.Get(name)))
                return false;
        }
        
        // Return true if everything is equal
        return true;
    }
};

/**
 * @brief Structure to represent the state of color, depth, and stencil buffers.
 */
struct RenderTargetBuffers
{
    // Constructor(s)
    // ----------------------------------------
    /// @brief Generate a buffer state with predefined buffer activation states.
    /// @param color Whether the color buffer is active (default: true).
    /// @param depth Whether the depth buffer is active (default: false).
    /// @param stencil Whether the stencil buffer is active (default: false).
    RenderTargetBuffers(bool color = true, bool depth = false, bool stencil = false)
        : Color(color), Depth(depth), Stencil(stencil)
    {}
    
    // Comparison operator to check if the current state is equal to the default state
    bool operator==(const RenderTargetBuffers& other) const
    {
        return Color == other.Color &&
               Depth == other.Depth &&
               Stencil == other.Stencil;
    }
    
    // Buffer state variables
    // ----------------------------------------
    bool Color;     ///< Indicates whether the color buffer is active.
    bool Depth;     ///< Indicates whether the depth buffer is active.
    bool Stencil;   ///< Indicates whether the stencil buffer is active.
};

} // namespace pixc

namespace std {

/**
 * @brief Hash function specialization for `BufferLayout`.
 *
 * @note: Allows `MetalRendererDescriptor` to be used as a key in unordered_map.
 */
template<>
struct hash<pixc::BufferLayout>
{
    /// @brief Generates a hash for a given `BufferLayout`.
    /// @param key The descriptor to hash.
    /// @return A combined hash of each element hash.
    size_t operator()(const pixc::BufferLayout& layout) const
    {
        size_t result = 0;
        for (const auto& name : layout.GetBufferOrder())
        {
            const auto& element = layout.Get(name);
            size_t nameHash = std::hash<std::string>()(name);
            size_t elementHash = std::hash<pixc::DataElement>()(element);
            
            result ^= nameHash ^ (elementHash << 1);
        }
        return result;
    }
};

} // namespace std
