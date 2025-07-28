/**
 * Linearize depth value from depth buffer to world space.
 *
 * @param depth Depth value to be linearized.
 * @param near Near clipping plane of the view frustum.
 * @param far Far clipping plane of the view frustum.
 *
 * @return Linearized depth value in world space.
 */
float linearizeDepth(float depth, float near, float far)
{
    // Convert depth from normalized device coordinates (NDC) back to clip space
    float z = depth * 2.0 - 1.0;
    
    // Calculate and return the linearized depth value using the formula
    // (2 * near * far) / (far + near - z * (far - near))
    return (2.0 * near * far) / (far + near - z * (far - near));
}
