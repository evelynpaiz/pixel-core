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
};

/**
 * Structure to represent the state of color, depth, and stencil buffers.
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
        : colorBufferActive(color), depthBufferActive(depth), stencilBufferActive(stencil)
    {}
    
    // Comparison operator to check if the current state is equal to the default state
    bool operator==(const RenderTargetBuffers& other) const
    {
        return colorBufferActive == other.colorBufferActive &&
               depthBufferActive == other.depthBufferActive &&
               stencilBufferActive == other.stencilBufferActive;
    }
    
    // Buffer state variables
    // ----------------------------------------
    bool colorBufferActive;     ///< Indicates whether the color buffer is active.
    bool depthBufferActive;     ///< Indicates whether the depth buffer is active.
    bool stencilBufferActive;   ///< Indicates whether the stencil buffer is active.
};

} // namespace pixc
