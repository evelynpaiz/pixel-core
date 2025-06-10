#pragma once

#include <glm/glm.hpp>

/**
 * Enumeration representing different types of primitives for rendering.
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
 * Enumeration representing face culling options.
 */
enum class FaceCulling
{
    Front, Back, FrontAndBack,
};

/**
 * Enumeration representing the different depth testing functions.
 */
enum class DepthFunction
{
    Always, Never, Less, Equal, LEqual, Greater, NotEqual, GEqual,
};
