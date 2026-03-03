// Quick checks for enabled shading features (bit flags stored in Material.Flags)
inline bool useHalfLambert(uint flags)
{
    // Bit 0: enable Half-Lambert diffuse lighting
    const uint FLAG_HALF_LAMBERT = 1u << 0;
    return (flags & FLAG_HALF_LAMBERT) != 0u;
}
inline bool useBlinnPhong(uint flags)
{
    // Bit 1: enable Blinn–Phong specular lighting
    const uint FLAG_BLINN_PHONG = 1u << 1;
    return (flags & FLAG_BLINN_PHONG) != 0u;
}

/**
 * Represents the minimal material data needed for lighting calculations.
 */
struct ShadingMaterial
{
    float3 Kd;              ///< Diffuse reflection coefficient.
    float3 Ks;              ///< Specular reflection coefficient.
    float Shininess;        ///< Shininess of the material (exponent for specular highlights).
    
    uint Flags;             ///< Shading flags.
};

/**
 * Represents the minimal light data needed for shading calculations.
 */
struct ShadingLight
{
    float4 Vector;                  ///< Light vector: directional (w=0) or positional (w=1).
    float3 Color;                   ///< Light color/intensity.
    
    float Linear;                   ///< Linear attenuation factor.
    float Quadratic;                ///< Quadratic attenuation factor.
    float DirectionalAttenuation;   ///< Constant attenuation for directional lights.
    
    float ShadowFactor;             ///< Shadow factor [0..1] (0 = fully lit, 1 = fully shadowed).
};

/**
 * Calculate the final color using the Phong shading model with consideration for shadows.
 *
 * @param position Position of the surface point.
 * @param normal Normal vector at the surface point.
 * @param viewPosition Position of the viewer/camera.
 * @param material Material properties.
 * @param light Light properties.
 *
 * @return Calculated color using the Phong shading model with shadows taken into account.
 */
inline float3 calculateColor(float3 position, float3 normal, float3 viewPosition,
                             ShadingMaterial material, ShadingLight light)
{
    // Calculate the surface vector(s)
    normal = normalize(normal);
    
    // Calculate the direction vectors
    float3 lightDirection = light.Vector.w == 1.0f ?
                                normalize(light.Vector.xyz - position) :     // positional light (.w = 1)
                                normalize(-light.Vector.xyz);                // directional light (.w = 0)
    float3 viewDirection = normalize(viewPosition - position);
    
    // Calculate the light radiance
    float3 radiance = light.Color * calculateAttenuation(position, light.Vector,
                                                         light.Linear, light.Quadratic,
                                                         light.DirectionalAttenuation);
    
    // Calculate the cosine between surface normal and light (clamped to [0,1])
    float cosTheta = saturate(dot(normal, lightDirection));
    // Calculate the diffuse reflection component either using:
    // -> Standard Lambert if flag is off
    // -> Half-Lambert (wrapped + squared) if flag is on
    const float diffuseWrap = 0.5f;
    float3 diffuse = useHalfLambert(material.Flags) ?
        float3(saturate(pow(cosTheta * diffuseWrap + (1.0f - diffuseWrap), 2.0f))):
        float3(cosTheta);
    // Apply diffuse material color
    diffuse *= material.Kd;
    
    // Calculate the specular reflection component either using:
    // -> Blinn-Phong if flag is on
    // -> Classic Phong otherwise
    float3 specular = useBlinnPhong(material.Flags) ?
        calculateBlinnPhongSpecular(normal, viewDirection, lightDirection, material.Ks, material.Shininess) :
        calculatePhongSpecular(normal, viewDirection, lightDirection, material.Ks, material.Shininess);
    // Fade out specular near the light terminator to avoid hard edges
    const float kSpecularEpsilon = 1e-4f;
    float specMask = smoothstep(kSpecularEpsilon, 1.0f, cosTheta);
    specular *= specMask;
    
    // Calculate the final color by combining ambient, diffuse, and specular components,
    // and modulating with the shadow factor
    float3 result = radiance * (1.0f - light.ShadowFactor) * (diffuse + specular);
    
    return result;
}
