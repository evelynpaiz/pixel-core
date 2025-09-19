/**
 * @brief Calculates the ambient illumination at a fragment using isotropic spherical harmonics
 *        for diffuse contribution and an environment map for specular reflection.
 *
 * @param position The fragment position in world space.
 * @param normal The normal vector at the fragment.
 * @param viewPosition The camera or view position in world space.
 * @param kd The diffuse reflectance (material's ambient color coefficient).
 * @param shininess The material shininess factor controlling the specular highlight size.
 * @param La The ambient light intensity scalar.
 * @param environmentMap The cubemap texture representing the environment for specular reflections.
 * @param irradianceMatrix The SHMatrix containing isotropic spherical harmonic coefficients for diffuse irradiance.
 *
 * @return The combined ambient color contribution as a vec3 (RGB).
 */
inline float3 calculateAmbientIsotropic(float3 position, float3 normal, float3 viewPosition,
                                        float3 kd, float shininess, float La,
                                        thread texturecube<float> &environmentMap,
                                        thread sampler &environmentMapSampler,
                                        SHMatrix irradianceMatrix)
{
    // Mathematical constants
    const float PI = 3.14159265359f;
    const float INV_PI = 1.0f / PI;
    
    // Diffuse contribution from SH irradiance
    float3 irradiance = calculateIrradiance(irradianceMatrix, normalize(normal), INV_PI);
    float3 diffuse = irradiance * kd;
    
    // Specular contribution from environment map
    float3 viewDirection = normalize(position - viewPosition);
    float3 reflectionDirection = reflect(viewDirection, normalize(normal));
    float3 specular = calculateSpecular(-viewDirection, reflectionDirection,
                                        environmentMap.sample(environmentMapSampler, toCubemapDir(reflectionDirection)).rgb,
                                        shininess);
    
    // Combine diffuse and specular with ambient intensity
    float3 ambient = La * (diffuse + specular);
    return ambient;
}
