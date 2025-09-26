#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/Pos.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "pixc/shaders/shared/structure/material/TextureMaterial.glsl"

// Specify the output color of the fragment shader
layout (location = 0) out vec4 color;

// Uniform buffer blocks
uniform Material u_Material;                ///< Material properties

// Input variables from the vertex shader
in vec3 v_Position;                         ///< Vertex position in object space

// Define constant variables
const vec2 invAtan = vec2(0.1591f, 0.3183f);

/**
 * Inverse tangent and spherical mapping function.
 *
 * This function calculates 2D texture coordinates (UV) for spherical mapping
 * based on a 3D vector 'v', which typically represents a normalized direction
 * vector from the origin to a point on a sphere. The resulting UV coordinates
 * are used for texture sampling on a spherical texture map.
 *
 * @param v The 3D vector representing a point on a unit sphere.
 *
 * @return The 2D texture coordinates (UV) mapped to the spherical map.
 */
vec2 sampleSphericalMap(vec3 v)
{
    // Calculate the azimuth (horizontal angle) and inclination (vertical angle)
    // of the input 3D vector and map them to UV coordinates.
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    
    // Scale the UV coordinates using the provided inverse tangent values.
    uv *= invAtan;
    
    // Offset the UV coordinates to ensure they are in the [0, 1] range.
    uv += 0.5f;
    
    return uv;
}

// Entry point of the fragment shader
void main()
{
    // Compute spherical UVs from the normalized fragment position, then sample the texture
    vec4 textureColor = texture(u_Material.TextureMap, sampleSphericalMap(normalize(v_Position)));
    // Output the sampled texture color as the final fragment color
    color = textureColor;
}
