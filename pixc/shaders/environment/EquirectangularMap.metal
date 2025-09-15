#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/material/TextureMaterial.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/Pos.vs.metal"

// ----------------------------------

// Define constant variables
constant float2 invAtan = float2(0.1591f, 0.3183f);

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
float2 sampleSphericalMap(float3 v)
{
    // Calculate the azimuth (horizontal angle) and inclination (vertical angle)
    // of the input 3D vector and map them to UV coordinates.
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    
    // Scale the UV coordinates using the provided inverse tangent values.
    uv *= invAtan;
    
    // Offset the UV coordinates to ensure they are in the [0, 1] range.
    uv += 0.5f;
    
    // Flip Y to account for Metal’s coordinate system
    uv.y = 1.0f - uv.y;
    
    return uv;
}

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              texture2d<float> u_Material_TextureMap [[ texture(MaterialIndex::TextureMap) ]],
                              sampler s_Material_TextureMap [[ sampler(MaterialIndex::TextureMap) ]])
{
    // Compute spherical UVs from the normalized fragment position, then sample the texture
    float4 color = u_Material_TextureMap.sample(s_Material_TextureMap, sampleSphericalMap(normalize(in.v_Position)));
    // Output the sampled texture color as the final fragment color
    return color;
}
