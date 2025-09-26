/**
 * @brief Calculates a bias value for shadow mapping based on the normal and light direction.
 *
 * This function computes a bias value that is used to offset the comparison between
 * the depth of a fragment and the depth stored in the shadow map. The bias helps to
 * prevent shadow acne and peter-panning artifacts.
 *
 * @param N The normalized surface normal of the fragment.
 * @param L The normalized direction vector from the fragment to the light source.
 * @param minBias The minimum bias value to be used.
 * @param maxBias The maximum bias value to be used.
 *
 * @return The calculated bias value based on the normal and light direction.
 */
float calculateBias(vec3 N, vec3 L, float minBias, float maxBias)
{
    // Calculate the dot product of the normal and light direction
    float dotProduct = dot(N, L);
    
    // Calculate the bias based on the dot product, ensuring it is within the min and max bounds
    return max(maxBias * (1.0f - dotProduct), minBias);
}
