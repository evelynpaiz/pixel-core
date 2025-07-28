/**
 * @brief Calculates a bias value based on the light distance and user-defined parameters.
 *
 * This function computes a bias value that is adjusted based on the distance of the light source
 * from the rendered fragment. The bias helps to prevent shadow artifacts and improve rendering quality.
 *
 * @param lightDistance The distance from the fragment to the light source.
 * @param farPlane The distance to the far plane of the frustum (typically the maximum render distance).
 * @param nearBias The bias value to be used when the light is close to the fragment.
 * @param farBias The bias value to be used when the light is far from the fragment.
 *
 * @return The calculated bias value based on the light distance.
 */
float calculateBias(float lightDistance, float farPlane, float nearBias, float farBias)
{
    // Normalize the light distance to the range [0, 1] based on the far plane
    float normalizedDistance = lightDistance / farPlane;
    
    // If the normalized distance is greater than 1.0, set it to 1.0
    normalizedDistance = min(normalizedDistance, 1.0);
    
    // Calculate the bias using a linear interpolation between nearBias and farBias
    return mix(nearBias, farBias, normalizedDistance);
}

