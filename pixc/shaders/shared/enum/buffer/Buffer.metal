
/**
 * @brief Defines indices for various buffer bindings used in shaders.
 */
enum BufferIndex {
    Vertices = 0,               ///< Index for the vertex buffer.
    
    // Uniforms:
    Transformations = 11,       ///< Index for the transformation matrices uniform buffer.
    Materials = 12              ///< Index for the material properties uniform buffer.
};

