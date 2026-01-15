/**
 * Calculate the specular reflection color using the Phong specular reflection formula.
 *
 * @param N Surface normal (normalized).
 * @param V View direction vector.
 * @param L Light direction (normalized, pointing *towards* the surface).
 * @param ks Specular reflection coefficient.
 * @param shininess Shininess factor for controlling specular highlight tightness.
 *
 * @return Calculated specular reflection color.
 */
inline float3 calculatePhongSpecular(float3 N, float3 V, float3 L,
                                     float3 ks, float shininess)
{
    // Compute reflection vector
    float3 R = normalize(2.0f * dot(L, N) * N - L);
    
    // Calculate the cosine of the angle between the view direction and reflection direction
    float cosPhi = saturate(dot(V, R));
    
    // Calculate the specular reflection color using the Phong specular reflection formula,
    // where ks is the specular reflection coefficient and shininess controls the tightness of the highlight
    float3 specularColor = pow(cosPhi, shininess) * ks;
    
    return specularColor;
}

/**
 * Calculate the specular reflection color using the Blinn–Phong model.
 *
 * @param N Surface normal (normalized).
 * @param V View direction vector.
 * @param L Light direction (normalized, pointing *towards* the surface).
 * @param ks Specular reflection coefficient.
 * @param shininess Shininess factor controlling highlight sharpness.
 *
 * @return Calculated Blinn–Phong specular reflection color.
 */
inline float3 calculateBlinnPhongSpecular(float3 N, float3 V, float3 L,
                                          float3 ks, float shininess)
{
    // Compute half vector between view and light direction
    float3 H = normalize(L + V);

    // Calculate the cosine of the angle between the normal and half vector
    float cosPhi = saturate(dot(N, H));

    // Calculate the specular reflection color using the Blinn–Phong specular reflection formula,
    // where ks is the specular reflection coefficient and shininess controls the tightness of the highlight
    float3 specularColor = pow(cosPhi, shininess) * ks;
    
    return specularColor;
}
