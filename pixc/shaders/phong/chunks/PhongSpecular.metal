/**
 * Calculate the specular reflection color using the Phong specular reflection formula.
 *
 * @param V View direction vector.
 * @param R Reflection direction vector.
 * @param ks Specular reflection coefficient.
 * @param shininess Shininess factor for controlling specular highlight tightness.
 *
 * @return Calculated specular reflection color.
 */
inline float3 calculateSpecular(float3 V, float3 R, float3 ks, float shininess)
{
    // Calculate the cosine of the angle between the view direction and reflection direction
    float cosPhi = saturate(dot(V, R));
    
    // Calculate the specular reflection color using the Phong specular reflection formula,
    // where ks is the specular reflection coefficient and shininess controls the tightness of the highlight
    float3 specularColor = pow(cosPhi, shininess) * ks;
    
    return specularColor;
}
