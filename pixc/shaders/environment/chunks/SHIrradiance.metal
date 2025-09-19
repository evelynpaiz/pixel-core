/**
 * Calculates irradiance using precomputed spherical harmonic matrices.
 *
 * @param v The input direction vector (typically a normalized normal or tangent vector).
 * @param irradianceMatrix A structure containing 3 SH irradiance matrices (Red, Green, Blue), each a 4x4 matrix.
 
 * @return The calculated irradiance as a vec3.  If no irradiance is available (all matrix elements close to zero),
 *         it returns a default irradiance of vec3(1.0).
 */
inline float3 calculateIrradiance(SHMatrix irradianceMatrix, float3 v, float scale)
{
    // Convert the input direction vector to homogeneous coordinates
    float4 vector = float4(v, 1.0);

    // Calculate irradiance for each color channel using SH matrices
    float3 irradiance;
    irradiance.r = dot(vector, irradianceMatrix.Red * vector);
    irradiance.g = dot(vector, irradianceMatrix.Blue * vector);
    irradiance.b = dot(vector, irradianceMatrix.Green * vector);
    
    // Normalize irradiance value (consistent with common SH scaling)
    irradiance *= scale;
    
    // Check if an irradiance map has been set.  If not, return default value to avoid black areas
    const float epsilon = 1e-6;
    if (fabs(irradiance.x) < epsilon &&
        fabs(irradiance.y) < epsilon &&
        fabs(irradiance.z) < epsilon)
    {
        irradiance = float3(1.0);
    }
    
    return irradiance;
}
