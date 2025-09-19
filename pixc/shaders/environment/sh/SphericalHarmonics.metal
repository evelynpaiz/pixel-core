#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/texture/TextureIndex.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/Pos.vs.metal"

// Define constant variables
constant float PI = 3.14159265359f;

// -----------------------------------------
// Sampling
// -----------------------------------------
/**
 * Generates a radical inverse using the Van der Corput sequence.
 *
 * This function takes an integer input `bits` and generates a radical inverse
 * using the Van der Corput sequence.
 *
 * @param bits An unsigned integer input.
 * @return The generated radical inverse as a floating-point value.
 */
inline float radicalInverseVDC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

/**
 * Generates a 2D Hammersley point.
 *
 * This function generates a 2D point in the Hammersley sequence using an index `i`
 * and the total number of samples `N`.
 *
 * @param i An unsigned integer index.
 * @param N An unsigned integer representing the total number of samples.
 * @return A 2D point in the Hammersley sequence as a vec2.
 */
inline float2 Hammersley(uint i, uint N)
{
    return float2(float(i)/float(N), radicalInverseVDC(i));
}

/**
 * Generate a uniformly distributed random sample on the unit sphere.
 *
 * @param U A 2D vector of random numbers (in the range [0, 1)) used to generate the sample.
 *
 * @return A 3D vector representing the uniformly sampled point on the unit sphere.
 */
inline float3 SampleSphereUniform(float2 U)
{
    // Maps U.x to [-1, 1] for use as z-coordinate
    float z = 1.0f - 2.0f * U.x;
    // Radius in x-y plane
    float r = sqrt(max(0.0f, 1.0f - z * z));

    // Maps U.y to [0, 2PI]
    float phi = 2.0 * PI * U.y;

    // Returns a point on a unit sphere
    return float3(r * cos(phi), r * sin(phi), z);
}

// -----------------------------------------
// Spherical Harmonics
// -----------------------------------------

/**
 * @brief Computes a spherical harmonic function value (implicit form, up to l=2).
 *
 * @param l The degree of the spherical harmonic (0 <= l <= 2).
 * @param m The order of the spherical harmonic (-l <= m <= l).
 * @param n The normalized direction vector (n.x, n.y, n.z).  phi and theta are computed internally from this vector.
 *
 * @return The computed value of the spherical harmonic function Y(l, m, theta, phi).
 *
 * @note Values taken from paper "An Efficient Representation for Irradiance Environment Maps".
 */
inline float SH(int l, int m, float3 n)
{
    // Handle the base case for l=0, m=0
    if (l == 0 && m == 0) return 0.282095f;
    // Handle the cases for l=1
    if (l == 1) return 0.488603f * ((m == -1) ? n.y : ((m == 0) ? n.z : n.x));
    // Handle the cases for l=2
    if (l == 2)
    {
        if (m == 0) return 0.315392f * (3.0f * n.z * n.z - 1.0f);
        if (m == 2) return 0.546274f * (n.x * n.x - n.y * n.y);
        return 1.092548f * ((m == 1) ? (n.x * n.z) : ((m == -1) ? (n.y * n.z) : (n.x * n.y)));
    }
    return 0.0f;
}

// Declare the SH coefficients as a constant array
constant int2 indices[9] = {
    int2(0, 0),  int2(1, -1), int2(1,  0),
    int2(1, 1),  int2(2, -2), int2(2, -1),
    int2(2, 0),  int2(2,  1), int2(2,  2)
};

// -----------------------------------------

// Include additional functions
#import "pixc/shaders/shared/utils/CubemapDir.metal"

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              texturecube<float> u_Material_TextureMap [[ texture(TextureIndex::TextureMap) ]],
                              sampler s_Material_TextureMap [[ sampler(TextureIndex::TextureMap) ]])
{
    // Get current pixel location
    int2 pixelCoord = int2(in.Position.xy);
    
    // Get the spherical harmonics indices
    int indx = pixelCoord.y * 3 + pixelCoord.x;
    int l = indices[indx].x;
    int m = indices[indx].y;
    
    // Define the initial values
    float3 Llm = float3(0.0);
    
    // Integrate over the hemisphere
    const uint SAMPLE_COUNT = 512u;
    const float deltaW = 4.0f * PI / float(SAMPLE_COUNT);
    
    for (uint i = 0; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 wi = SampleSphereUniform(Xi);
        
        float3 Li = u_Material_TextureMap.sample(s_Material_TextureMap, toCubemapDir(wi)).rgb;
        
        Llm += SH(l, m, wi) * Li * deltaW;
    }
    
    return float4(Llm, 1.0f);
}
