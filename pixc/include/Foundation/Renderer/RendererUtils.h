#pragma once

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Enumeration representing different types of primitives for rendering.
 */
enum class PrimitiveType
{
    Point,          ///< Points, each vertex represents a point.
    Line,           ///< Lines, each pair of vertices forms a line segment.
    LineStrip,      ///< Line strip, consecutive vertices form connected lines.
    Triangle,       ///< Triangles, each group of three vertices forms a triangle.
    TriangleStrip   ///< Triangle strip, consecutive vertices form connected triangles.
};

/**
 * @brief Enumeration representing face culling options.
 */
enum class FaceCulling
{
    Front, Back, FrontAndBack,
};

/**
 * @brief Enumeration representing the different depth testing functions.
 */
enum class DepthFunction
{
    None, Always, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual,
};

/**
 * @brief Describes the configuration for depth testing.
 */
struct DepthDescriptor
{
    ///< Whether depth testing and writing is enabled.
    bool Enabled = false;
    ///< The depth comparison function (e.g., Less, Greater, Equal).
    DepthFunction Function = DepthFunction::Less;
    
    /// @brief Equality operator for comparing two DepthDescriptor instances.
    /// @param other Another descriptor to compare against.
    /// @return True if both enabled state and compare function match.
    bool operator==(const DepthDescriptor& other) const
    {
        return Enabled == other.Enabled &&
               Function == other.Function;
    }
};

} // namespace pixc


namespace std {

/**
 * @brief Hash function specialization for DepthDescriptor.
 *
 * @note: Allows DepthDescriptor to be used as a key in unordered_map or other
 * hash-based containers.
 */
template<>
struct hash<pixc::DepthDescriptor> {
    
    /// @brief Generates a hash for a given DepthDescriptor.
    /// @param key The depth descriptor to hash.
    /// @return A combined hash of its Enabled flag and CompareFunction.
    size_t operator()(const pixc::DepthDescriptor& key) const {
        return std::hash<bool>()(key.Enabled) ^
               (std::hash<int>()(static_cast<int>(key.Function)) << 1);
    }
};

} // namespace std
