/**
 * @brief Generates the M matrix for irradiance calculation from spherical harmonic coefficients (isotropic).
 *
 * @param colorIndex The index of the color channel (0 = red, 1 = green, 2 = blue).
 * @param L An array containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22).
 *
 * @return The 4x4 matrix M for the specified color channel.
 */
mat4 generateM(int colorIndex, const vec3 L[9])
{
    float c1 = 0.429043f;
    float c2 = 0.511664f;
    float c3 = 0.743125f;
    float c4 = 0.886227f;
    float c5 = 0.247708f;

    mat4 M = mat4(0.0); // Initialize M to zero.

    // Construct M using the precomputed L coefficients
    // Access the appropriate color channel using colorIndex
    M[0][0] = c1 * L[8][colorIndex];    // L22
    M[0][1] = c1 * L[4][colorIndex];    // L2-2
    M[0][2] = c1 * L[7][colorIndex];    // L21
    M[0][3] = c2 * L[3][colorIndex];    // L11

    M[1][0] = M[0][1];                 // Symmetry
    M[1][1] = -M[0][0];                // Symmetry
    M[1][2] = c1 * L[5][colorIndex];    // L2-1
    M[1][3] = c2 * L[1][colorIndex];    // L1-1

    M[2][0] = M[0][2];                 // Symmetry
    M[2][1] = M[1][2];                 // Symmetry
    M[2][2] = c3 * L[6][colorIndex];    // L20
    M[2][3] = c2 * L[2][colorIndex];    // L10

    M[3][0] = M[0][3];                 // Symmetry
    M[3][1] = M[1][3];                 // Symmetry
    M[3][2] = M[2][3];                 // Symmetry
    M[3][3] = c4 * L[0][colorIndex] - c5 * L[6][colorIndex]; // L00 and L20

    return M;
}
