#pragma once

#include "Foundation/Renderer/Shader/Shader.h"
#include "Foundation/Renderer/Buffer/Buffer.h"
#include "Foundation/Renderer/Buffer/FrameBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Describes the configuration for rendering.
 */
struct MetalRenderDescriptor
{
    ///< Shader program to be used for rendering.
    std::shared_ptr<Shader> Shader;
    ///< Description of the attributes of the vertices.
    BufferLayout VertexLayout;
    ///< Description of the attachments rendered into.
    AttachmentSpecification Attachments;
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Equality operator for comparing two render descriptor.
    /// @param other Another descriptor to compare against.
    /// @return True if both descriptors match.
    bool operator==(const MetalRenderDescriptor& other) const
    {
        return Shader->GetName() == other.Shader->GetName() &&
               VertexLayout == other.VertexLayout &&
               Attachments == other.Attachments;
    }
};

/**
 * @brief Describes the configuration for depth testing.
 */
struct MetalDepthDescriptor
{
    ///< Whether depth testing and writing is enabled.
    bool Enabled = false;
    ///< The depth comparison function (e.g., Less, Greater, Equal).
    DepthFunction Function = DepthFunction::Less;
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Equality operator for comparing two depth descriptor.
    /// @param other Another descriptor to compare against.
    /// @return True if both descriptors match.
    bool operator==(const MetalDepthDescriptor& other) const
    {
        return Enabled == other.Enabled &&
               Function == other.Function;
    }
};

} // namespace pixc

namespace std {

/**
 * @brief Hash function specialization for `MetalDepthDescriptor`.
 *
 * @note: Allows `MetalDepthDescriptor` to be used as a key in unordered_map or other
 * hash-based containers.
 */
template<>
struct hash<pixc::MetalDepthDescriptor>
{
    
    /// @brief Generates a hash for a given `MetalDepthDescriptor`.
    /// @param key The depth descriptor to hash.
    /// @return A combined hash of its `Enabled` flag and `CompareFunction`.
    size_t operator()(const pixc::MetalDepthDescriptor& key) const
    {
        return std::hash<bool>()(key.Enabled) ^
               (std::hash<int>()(static_cast<int>(key.Function)) << 1);
    }
};

/**
 * @brief Hash function specialization for `MetalRendererDescriptor`.
 *
 * @note: Allows `MetalRendererDescriptor` to be used as a key in unordered_map
 * or other hash-based containers.
 */
template<>
struct std::hash<pixc::MetalRenderDescriptor>
{
    /// @brief Generates a hash for a given `MetalRendererDescriptor`.
    /// @param key The descriptor to hash.
    /// @return A combined hash of its `Shader` name, `VertexLayout`, and `Attachments`.
    size_t operator()(const pixc::MetalRenderDescriptor& key) const
    {
        // Shader hash based on its name
        size_t hash = std::hash<std::string>()(key.Shader->GetName());

        // Combine with VertexLayout hash (assuming it's hashable)
        hash ^= std::hash<pixc::BufferLayout>()(key.VertexLayout) << 1;

        // Combine with AttachmentSpecification hash (assuming it's hashable)
        hash ^= std::hash<pixc::AttachmentSpecification>()(key.Attachments) << 2;

        return hash;
    }
};

} // namespace std
