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
    // Calculate and return the linearized depth value
    return (2.0f * near * far) / (far + near - depth * (far - near));
}
