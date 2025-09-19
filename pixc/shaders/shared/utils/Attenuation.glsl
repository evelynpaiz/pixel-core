/**
 * Calculate the attenuation factor for a light source.
 *
 * @param position The position of the object or surface receiving light.
 * @param lightVector The vector from the object to the light source, including
 *                   the direction and distance information.
 *
 * @return The attenuation factor, which represents how much the light intensity diminishes with distance.
 *         A value of 1.0f means no attenuation, and smaller values mean greater attenuation.
 */
float calculateAttenuation(vec3 position, vec4 lightVector,
                           float linear, float quadratic, float maxValue)
{
    // For directional lights, no distance attenuation is applied,
    // so we return a fixed attenuation factor of 0.8f
    if (lightVector.w == 0.0f)
        return maxValue;
    
    // Calculate attenuation using the inverse square law formula.
    // This simulates how light intensity diminishes with distance.
    float lightDistance = length(lightVector.xyz - position);
    float attenuation = 1.0f / (1.0f + linear * lightDistance + quadratic * (lightDistance * lightDistance));
    
    return min(maxValue, attenuation);
}
