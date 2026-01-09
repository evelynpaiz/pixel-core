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
vec3 calculatePhongSpecular(vec3 N, vec3 V, vec3 L,
                            vec3 ks, float shininess)
{
    // Compute reflection vector
    vec3 R = normalize(2.0f * dot(L, N) * N - L);
    
    // Calculate the cosine of the angle between the view direction and reflection direction
    float cosPhi = saturate(dot(V, R));
    
    // Calculate the specular reflection color using the Phong specular reflection formula,
    // where ks is the specular reflection coefficient and shininess controls the tightness of the highlight
    vec3 specularColor = pow(cosPhi, shininess) * ks;
    
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
vec3 calculateBlinnPhongSpecular(vec3 N, vec3 V, vec3 L, vec3 ks, float shininess)
{
    // Compute half vector between view and light direction
    vec3 H = normalize(L + V);

    // Calculate the cosine of the angle between the normal and half vector
    float cosPhi = saturate(dot(N, H));

    // Calculate the specular reflection color using the Blinn–Phong specular reflection formula,
    // where ks is the specular reflection coefficient and shininess controls the tightness of the highlight
    vec3 specularColor = pow(cosPhi, shininess) * ks;
    
    return specularColor;
}
