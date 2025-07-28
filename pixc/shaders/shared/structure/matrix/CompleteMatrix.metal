/**
 * Represents transformation matrices for rendering.
 */
struct Transform {
    float4x4 Model;         ///< Model matrix for transforming object vertices to world space.
    float4x4 View;          ///< View matrix for transforming world space to camera space.
    float4x4 Projection;    ///< Projection matrix for transforming camera space to clip space.
    
    float3x3 Normal;        ///< Normal matrix for transforming normals to world space.
    float4x4 Texture;       ///< Texture matrix for transforming position to [0, 1] range for texture sampling.
};
