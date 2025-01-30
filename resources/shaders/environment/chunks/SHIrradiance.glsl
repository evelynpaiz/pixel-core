/**
 * Calculates irradiance using precomputed spherical harmonic matrices.
 *
 * @param v The input direction vector (typically a normalized normal or tangent vector).
 * @param irradianceMatrices An array of 3 4x4 matrices representing the SH coefficients for each color channel
 *                           ([0] = red, [1] = green, [2] = blue).
 
 * @return The calculated irradiance as a vec3.  If no irradiance is available (all matrix elements close to zero),
 *         it returns a default irradiance of vec3(1.0).
 */
vec3 calculateIrradiance(mat4 irradianceMatrices[3], vec3 v, float scale)
{
    // Convert the input direction vector to homogeneous coordinates.
    vec4 vector = vec4(v, 1.0);

    // Calculate irradiance for each color channel using SH matrices.
    vec3 irradiance;
    irradiance.r = dot(vector, irradianceMatrices[0] * vector);
    irradiance.g = dot(vector, irradianceMatrices[1] * vector);
    irradiance.b = dot(vector, irradianceMatrices[2] * vector);

    // Normalize irradiance value (consistent with common SH scaling).
    irradiance *= scale;

    // Check if an irradiance map has been set.  If not, return default value to avoid black areas.
    const float epsilon = 1e-6;
    if (all(lessThan(abs(irradiance), vec3(epsilon))))
        irradiance = vec3(1.0);

    return irradiance;
}
