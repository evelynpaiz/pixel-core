// Ref code: https://graphics.stanford.edu/papers/envmap/
// Ref env. maps: https://www.pauldebevec.com/Probes/

#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/common/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/common/vertex/P.vs.glsl"

#shader fragment
#version 330 core

/**
 * Represents the material properties of an object.
 */
struct Material {
    sampler2D TextureMap;                 ///< Texture map applied to the material.
};

// Uniform buffer blocks
uniform Material u_Material;    // Material properties

layout (location = 0) out vec3 Llm;

/// A constant representing the mathematical constant Pi.
const float PI = 3.14159265359f;
const float INV_PI = 1.0f / PI;

const float SQRT2 = sqrt(2.0f);

// -----------------------------------------
// Spherical Harmonics
// -----------------------------------------

/**
 * @brief Calculates a normalized factorial ratio.
 *
 * It is specifically designed to efficiently compute the ratio (a-b)!/(a+b)!.
 *
 * @param a The first integer input (must be >= b).
 * @param b The second integer input (must be >= 0).
 *
 * @return The computed factorial ratio. Returns 0.0 if b > a.
 */
float divfact(int a, int b)
{
    // Ensure b is non-negative
    b = abs(b);
    // Handle invalid cases where b > a (prevent infinite loops)
    if (b > a) return 0.0;
    // Handle separately to prevent infinite for loop
    if (b == 0) return 1.0;

    // Compute the div factorial
    float v = 1.0f;
    for (int x = (a - b + 1); x <= a + b; ++x) 
    {
        //Prevent multiplying by 0 in case (a+b) > factorial's range
        if(x == 0) continue;
        v *= float(x);
    }
    return 1.0f/v;
}

/**
 * @brief Calculates the normalization constant for spherical harmonics.
 *
 * Computes the normalization constant K(l, m) used in spherical harmonic functions.
 *
 * @param l The degree of the spherical harmonic (l >= 0).
 * @param m The order of the spherical harmonic (-l <= m <= l).
 *
 * @return The normalization constant K(l, m).
 */
float K(int l, int m)
{
    return sqrt((2.0f * float(l) + 1.0f) * 0.25f * INV_PI * divfact(l, m));
}

/**
 * @brief Evaluates an associated Legendre polynomial.
 *
 * @param l The degree of the associated Legendre polynomial (l >= 0).
 * @param m The order of the associated Legendre polynomial (0 <= m <= l).
 * @param x The value at which to evaluate the polynomial (-1 <= x <= 1).
 *
 * @return The computed value of the associated Legendre polynomial P(l, m, x).
 */
float P(int l, int m, float x)
{
    // Initialize the first associated Legendre polynomial value for m
    float pmm = 1.0f;
    // Handle the case where m is positive
    if (m > 0)
    {
        // Compute a value proportional to the square root of (1-x²)
        float somx2 = sqrt((1.0f - x) * (1.0f + x));
        if (m == 1)
            pmm = somx2;
        else
        {
            // Initialize the factoria (it will be increased by 2 at each iteration)
            float fact = 1.0f;
            // Compute the associated Legendre polynomial value
            for (int i = 1; i <= m; i++)
            {
                pmm *= (-fact) * somx2;
                fact += 2.0f;
            }
        }
    }
    // Handle the case where l equals m
    if (l == m) return pmm;
    
    //Compute the associated Legendre polynomial value for l = m+1
    float pmmp1 = x * (2.0f * m + 1.0f) * pmm;
    //Handle the case where l equals m+1
    if (l == (m + 1)) return pmmp1;
    
    // Initialize the associated Legendre polynomial value for l
    float pll = 0.0f;
    // Compute the associated Legendre polynomial value iteratively.
    // This is the main iterative computation of the associated Legendre polynomial
    for (int ll = (m + 2); ll <= l; ++ll)
    {
        pll = ((2.0f * float(ll) - 1.0f) * x * pmmp1 - (float(ll + m) - 1.0f) * pmm) / float(ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }
    //Return the associated Legendre polynomial value for a given l,m and x
    return pll;
}

/**
 * @brief Computes a spherical harmonic function value (explicit form).
 *
 * This function calculates the value of a real-valued spherical harmonic function Y(l, m, theta, phi)
 * using explicit calculations for the normalization constant and associated Legendre polynomial.
 *
 * @param l The degree of the spherical harmonic (l >= 0).
 * @param m The order of the spherical harmonic (-l <= m <= l).
 * @param theta The polar angle in radians (0 <= theta <= PI).
 * @param phi The azimuthal angle in radians (0 <= phi <= 2*PI).
 *
 * @return The computed value of the spherical harmonic function Y(l, m, theta, phi).
 */
float SH(int l, int m, float theta, float phi)
{
    if (m == 0)
        return K(l, m) * P(l, m, cos(theta));
    else if (m > 0)
        return SQRT2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
    
    return SQRT2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
}

// -----------------------------------------
// Spherical Harmonics (Explicit Form)
// -----------------------------------------

/**
 * @brief Computes a spherical harmonic function value (implicit form, up to l=2).
 *
 * This function calculates the value of a real-valued spherical harmonic function Y(l, m, theta, phi)
 * for degrees l up to 2, using pre-computed constants for efficiency.
 *
 * @param l The degree of the spherical harmonic (0 <= l <= 2).
 * @param m The order of the spherical harmonic (-l <= m <= l).
 * @param n The normalized direction vector (n.x, n.y, n.z).  phi and theta are computed internally from this vector.
 *
 * @return The computed value of the spherical harmonic function Y(l, m, theta, phi).
 *
 * @note Values taken from paper "An Efficient Representation for Irradiance Environment Maps".
 */
float SH(int l, int m, vec3 n)
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

/**
 * @brief Computes the sinc function with a safety check for division by zero.
 *
 * This function approximates the sinc function (sin(x)/x), handling the case when x is close
 * to zero to avoid division by zero errors.
 *
 * @param x The input value for the sinc function.
 *
 * @return The computed sinc value. Returns 1.0 if the absolute value of x is less than 1e-4.
 */
float sinc(float x)
{
    return abs(x) < 1e-4 ? 1.0f : sin(x) / x;
}

// Declare the SH coefficients as a constant array
const ivec2 indices[9] = ivec2[9](
    ivec2(0, 0),  ivec2(1, -1), ivec2(1,  0),
    ivec2(1, 1),  ivec2(2, -2), ivec2(2, -1),
    ivec2(2, 0),  ivec2(2,  1), ivec2(2,  2)
);

// Entry point of the fragment shader
void main()
{
    // Get current pixel location
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    
    // Get the spherical harmonics indices to be used in the computation
    int indx = pixelCoord.y * 3 + pixelCoord.x;
    int l = indices[indx].x;
    int m = indices[indx].y;
    
    // Get the size of the environment map
    int mipLevel = 2;
    ivec2 size = textureSize(u_Material.TextureMap, mipLevel);
    int width = size.x;
    int height = size.y;
    
    int samples = 0;
    // Integrate over the sphere of the upcomming light
    Llm = vec3(0.0f);
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // Values for equirectangular mapping (currently not working)
            //float phi = ((float(i) / width) - 0.5f)  * 2.0f * PI;
            //float theta = ((float(j) / height)) * PI;
            
            float u = (float(i) - float(width)/2.0f) / (float(width)/2.0f);
            float v = (float(j) - float(height)/2.0f) / (float(height)/2.0f);
            
            float r = sqrt(u * u + v * v);
            if (r > 1.0)
                continue;
            
            float phi = (u != 0.0f) ? atan(v, u) : atan(v, 1e-4);
            float theta = PI * r;
            
            vec3 wi = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            
            vec2 coords = vec2(float(i)/width, float(j)/height);
            vec3 Li = textureLod(u_Material.TextureMap, coords, mipLevel).rgb;
                        
            Llm += SH(l, m, wi) * Li * sinc(theta);
            samples++;
            
            /*
             * Use this code to test the SH direct computation.
            if  (indx < 9)
            {
                Llm += SH(l, m, theta, phi) * Li * sinc(theta);
                samples++;
            }
            */
        }
    }
    
    Llm *= 4.0f * PI;
    Llm /= float(width * height);
}
