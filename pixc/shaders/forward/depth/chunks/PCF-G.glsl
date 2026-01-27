/**
 * Percentage Closer Filtering (PCF) with Gaussian weighting for shadow mapping.
 *
 * @param shadowMap The sampler2D texture of the shadow map.
 * @param projectionCoord The normalized device coordinates of the fragment.
 * @param kernelSize The size of the PCF kernel.
 * @param sigma The standard deviation of the Gaussian distribution.
 *
 * @return The shadow value for the fragment.
 */
float PCF(sampler2D shadowMap, vec3 projectionCoord, int kernelSize, float sigma)
{
    // Get depth of current fragment from light's perspective
    float currentDepth = projectionCoord.z;
    
    // Calculate the size of a texel in the shadow map
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    // Calculate half of the kernel size
    int halfKernel = kernelSize / 2;
    
    // Initialize shadow value
    float shadow = 0.0f;
    
    // Loop over the PCF kernel
    for(int x = -halfKernel; x <= halfKernel; ++x)
    {
       for(int y = -halfKernel; y <= halfKernel; ++y)
       {
           // Compute an offset based on the current kernel sample
           vec2 sampleOffset = vec2(x, y) * texelSize;
           
           // Sample the shadow map at the offset position
           float pcfDepth = texture(shadowMap, projectionCoord.xy + sampleOffset).r;
           
           // Calculate the squared distance from the center of the kernel
           float squaredDistance = float(x * x + y * y);
           
           // Apply Gaussian weight to the sample based on the squared distance
           float weight = exp(-0.5 * (squaredDistance / (sigma * sigma)));
           
           // Update the shadow value using the sampled depth and weight
           shadow += currentDepth > pcfDepth ? weight : 0.0f;
       }
    }
    
    // Normalize the shadow value by the sum of weights in the kernel
    shadow /= float(kernelSize * kernelSize);
    
    return shadow;
}
