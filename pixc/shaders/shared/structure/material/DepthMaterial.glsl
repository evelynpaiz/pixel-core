/**
 * Represents the material properties of an object.
 */
struct Material
{
    sampler2D DepthMap;         ///< Depth map to be displayed
    
    uint Linearize;             ///< Whether to linearize depth. Disable for orthographic cameras.
    
    float NearPlane;            ///< Near plane for linearizing depth
    float FarPlane;             ///< Far plane for linearizing depth
};
