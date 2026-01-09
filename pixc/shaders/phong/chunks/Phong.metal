/**
 * Calculate the final color using the Phong shading model with consideration for shadows.
 *
 * @param position Position of the surface point.
 * @param normal Normal vector at the surface point.
 * @param viewPosition Position of the viewer/camera.
 * @param lightVector Vector pointing towards the light source (directional or positional).
 * @param lightColor Color of the light source.
 * @param kd Diffuse reflection coefficient.
 * @param ks Specular reflection coefficient.
 * @param shininess Shininess of the material (exponent for specular highlights).
 * @param shadow Shadow factor, ranging from 0.0 to 1.0, indicating the amount of shadowing.
 *               0.0 represents fully lit, while 1.0 represents fully shadowed.
 * @param lightLinear  Linear attenuation factor for the light source.
 * @param lightQuadratic Quadratic attenuation factor for the light source.
 * @param maxAttenuation Maximum attenuation distance for the light source.
 *
 * @return Calculated color using the Phong shading model with shadows taken into account.
 */
inline float3 calculateColor(float3 position, float3 normal, float3 viewPosition, float4 lightVector,
                             float3 lightColor, float3 kd, float3 ks, float shininess, float shadow,
                             float lightLinear, float lightQuadratic, float maxAttenuation)
{
    // Calculate the surface vector(s)
    normal = normalize(normal);
    
    // Calculate the direction vectors
    float3 lightDirection = lightVector.w == 1.0f ?
                          normalize(lightVector.xyz - position)         // positional light (.w = 1)
                          : normalize(-lightVector.xyz);                // directional light (.w = 0)
    float3 viewDirection = normalize(viewPosition - position);
    
    // Calculate the light radiance
    float3 radiance = lightColor * calculateAttenuation(position, lightVector,
                                                        lightLinear, lightQuadratic, maxAttenuation);
    
    // Calculate the cosine of the angle
    float cosTheta = saturate(dot(normal, lightDirection));
    float angleFalloff = smoothstep(1e-4f, 0.2f, cosTheta);
    
    // Calculate the diffuse reflection component using the Lambertian cosine law
    float3 diffuse = cosTheta * kd;
    
    // Calculate the specular reflection component using the Phong-Blinn specular reflection formula
    // NOTE: Use the function calculatePhongSpecular(...) for the original Phong model
    float3 specular = angleFalloff * calculateBlinnPhongSpecular(normal, viewDirection, lightDirection, ks, shininess);
    
    // Calculate the final color by combining ambient, diffuse, and specular components,
    // and modulating with the shadow factor
    float3 result = radiance * (1.0 - shadow) * (diffuse + specular);
    
    return result;
}
