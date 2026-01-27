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

    /*
    // NOTE: legacy OpenGL method (for perspective projection with default [-1,1] NDC):
     
    // Convert depth from normalized device coordinates (NDC) back to clip space
    float z = depth * 2.0f - 1.0f;
    
    // Calculate and return the linearized depth value using the classic formula:
    // return (2.0f * near * far) / (far + near - z * (far - near));
    */
}
