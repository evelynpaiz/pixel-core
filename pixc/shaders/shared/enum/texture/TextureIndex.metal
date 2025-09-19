/**
 * @brief Defines indices for various buffer bindings used in shaders.
 */
enum TextureIndex
{
    EnvironmentMap  = 0,        ///< Environment cubemap (used for IBL/specular reflections)
    TextureMap      = 1,        ///< Main texture (albedo)
    DiffuseMap      = 1,        ///< Optional diffuse map (may overlap with TextureMap)
    SpecularMap     = 2,        ///< Specular map (controls intensity / roughness)
    
    ShadowMap0      = 5,        ///< Shadow map for light 0
    ShadowMap1      = 6,        ///< Shadow map for light 1
    ShadowMap2      = 7,        ///< Shadow map for light 2
    ShadowMap3      = 8         ///< Shadow map for light 3
};
