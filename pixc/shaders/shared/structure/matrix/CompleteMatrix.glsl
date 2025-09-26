/**
 * Represents transformation matrices for rendering.
 */
struct Transform
{
    mat4 Model;         ///< Model matrix for transforming object vertices to world space.
    mat4 View;          ///< View matrix for transforming world space to camera space.
    mat4 Projection;    ///< Projection matrix for transforming camera space to clip space.
    
    mat4 Normal;        ///< Normal matrix for transforming normals to world space.
    mat4 Texture;       ///< Texture matrix for transforming position to [0, 1] range for texture sampling.
};
