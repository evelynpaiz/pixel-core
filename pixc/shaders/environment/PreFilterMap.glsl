#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Input vertex attributes
layout (location = 0) in vec4 a_Position;   ///< Vertex position in object space

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

// Outputs to fragment shader
out vec3 v_Position;                        ///< Vertex position in world space

// Entry point of the vertex shader
void main()
{
    // Transform the vertex position and normal from object space to world space
    vec4 worldPosition = u_Transform.Model * a_Position;
    
    // Perspective divide to get vertex position in normalized device coordinates
    v_Position = worldPosition.xyz / worldPosition.w;
    
    // Calculate the final position of the vertex in clip space
    gl_Position = u_Transform.Projection * u_Transform.View * worldPosition;
}

#shader fragment
#version 330 core

/**
 * Represents the material properties of an object.
 */
struct Material {
    float Roughness;                        ///< Material roughness.
    
    samplerCube TextureMap;                 ///< Texture map applied to the material.
};

// Specify the output color of the fragment shader
layout (location = 0) out vec4 color;

// Uniform buffer blocks
uniform Material u_Material;                ///< Material properties

// Input variables from the vertex shader
in vec3 v_Position;                         ///< Vertex position in world space

// Define constant variables
const float PI = 3.14159265359f;

/**
 * Generates a radical inverse using the Van der Corput sequence.
 *
 * This function takes an integer input `bits` and generates a radical inverse
 * using the Van der Corput sequence.
 *
 * @param bits An unsigned integer input.
 * @return The generated radical inverse as a floating-point value.
 */
float radicalInverseVDC(uint bits)
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
vec2 hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), radicalInverseVDC(i));
}

/**
 * Importance samples the GGX (Trowbridge-Reitz) distribution.
 *
 * This function importance samples the GGX distribution for microfacet-based
 * reflection with given random variables `xi`, surface normal `normal`, and
 * roughness parameter `roughness`.
 *
 * @param xi A 2D random variable (typically from the Hammersley sequence).
 * @param normal The surface normal vector.
 * @param roughness The roughness parameter (0.0 for smooth, 1.0 for rough).
 * @return A sampled direction in world space as a vec3.
 */
vec3 importanceSampleGGX(vec2 xi, vec3 normal, float roughness)
{
    float a = roughness * roughness;
    
    float phi = 2.0f * PI * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // From spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // From tangent-space vector to world-space sample vector
    vec3 up = abs(normal.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;
    return normalize(sampleVec);
}

// Entry point of the fragment shader
void main()
{
    // Calculate the normal vector from the vertex position in world space
    vec3 normal = normalize(v_Position);
    vec3 reflection = normal;
    vec3 viewDirection = reflection;
    
    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;
    
    // Initialize the prefiltered value
    vec3 prefiltered = vec3(0.0);
    
    // Iterate over phi (azimuthal angle) and theta (polar angle)
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 xi = hammersley(i, SAMPLE_COUNT);
        
        vec3 halfVector  = importanceSampleGGX(xi, normal, u_Material.Roughness);
        vec3 lightDirection  = normalize(2.0 * dot(viewDirection, halfVector) * halfVector - viewDirection);
        
        float cosTheta = max(dot(normal, lightDirection), 0.0);
        if(cosTheta > 0.0)
        {
            prefiltered += texture(u_Material.TextureMap, lightDirection).rgb * cosTheta;
            totalWeight += cosTheta;
        }
    }
    
    // Normalize the prefiltered value
    prefiltered = prefiltered / totalWeight;
    
    // Set the output color to the irradiance
    color = vec4(prefiltered, 1.0f);
}
