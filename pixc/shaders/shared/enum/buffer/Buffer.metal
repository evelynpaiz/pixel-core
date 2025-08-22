
/**
 * @brief Defines indices for various buffer bindings used in shaders.
 */
enum BufferIndex
{
    VertexBuffer       = 0,         ///< Vertex attribute data for the mesh.

    // Uniform buffers:
    TransformBuffer    = 11,        ///< Transformation matrices (model, view, projection, etc.).
    MaterialBuffer     = 12,        ///< Material properties (colors, textures, shading parameters).
    ViewBuffer         = 13,        ///< Camera/view data (position, orientation, projection settings).
    LightBuffer        = 14,        ///< Light source data (positions, intensities, colors).
    EnvironmentBuffer  = 15         ///< Environment lighting data (ambient, IBL, skybox parameters).
};
