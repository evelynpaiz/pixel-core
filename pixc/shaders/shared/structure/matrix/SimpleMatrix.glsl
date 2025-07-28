/**
 * Represents transformation matrices for rendering.
 */
struct Transform {
    mat4 Model;         ///< Model matrix for transforming object vertices to world space.
    mat4 View;          ///< View matrix for transforming world space to camera space.
    mat4 Projection;    ///< Projection matrix for transforming camera space to clip space.
};
