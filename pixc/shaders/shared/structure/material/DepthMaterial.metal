/**
 * Represents the material properties of an object.
 */
struct Material
{
    unsigned int Linearize;         ///< Whether to linearize depth. Disable for orthographic cameras.
    
    float NearPlane;                ///< Near plane for linearizing depth
    float FarPlane;                 ///< Far plane for linearizing depth
    
    float _pad;                     ///< 4 bytes padding to make 16 bytes
};
