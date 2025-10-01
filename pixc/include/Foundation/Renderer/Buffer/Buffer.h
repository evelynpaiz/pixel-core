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
 * @brief Bitmask flags representing which render target buffers to clear.
 */
enum class RenderTargetMask : uint8_t
{
    None    = 0,          ///< No buffer is cleared.
    Color   = 1 << 0,     ///< Clear the color buffer.
    Depth   = 1 << 1,     ///< Clear the depth buffer.
    Stencil = 1 << 2      ///< Clear the stencil buffer.
};

/**
 * @brief Bitwise OR operator for combining buffer flags.
 */
inline RenderTargetMask operator|(RenderTargetMask a, RenderTargetMask b)
{
    return static_cast<RenderTargetMask>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

/**
 * @brief Bitwise AND operator for checking overlapping buffer flags.
 */
inline RenderTargetMask operator&(RenderTargetMask a, RenderTargetMask b)
{
    return static_cast<RenderTargetMask>(
        static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
    );
}

/**
 * @namespace utils::graphics::gl
 * @brief Utility functions related to Metal rendering operations.
 */
namespace utils { namespace graphics {

/**
 * @brief Check if a specific buffer flag is active in the bitmask.
 *
 * @param mask Bitmask of render targets (e.g., color, depth, stencil).
 * @param target Specific render target to check.
 * @return true if the target is active in the mask.
 */
inline bool IsBufferActive(RenderTargetMask mask, RenderTargetMask target)
{
    return (static_cast<uint8_t>(mask) & static_cast<uint8_t>(target)) != 0;
}

} // namespace graphics
} // namespace utils
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
