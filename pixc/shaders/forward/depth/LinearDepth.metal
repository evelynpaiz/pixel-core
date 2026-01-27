#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/texture/TextureIndex.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/Tex.vs.metal"

// ----------------------------------

// Include material properties
#import "pixc/shaders/shared/structure/material/DepthMaterial.metal"

// Include linearization function for pespective projections
#import "pixc/shaders/forward/depth/chunks/LinearizeDepth.metal"

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              constant Material &u_Material [[ buffer(BufferIndex::MaterialBuffer) ]],
                              texture2d<float> u_Material_DepthMap [[ texture(TextureIndex::TextureMap) ]],
                              sampler s_Material_DepthMap [[ sampler(TextureIndex::TextureMap) ]])
{
    // Define the near and far planes for depth linearization
    float nearPlane = u_Material.NearPlane;
    float farPlane =  u_Material.FarPlane;
    
    // Sample the depth value from the texture using the provided texture coordinates
    float depthValue = u_Material_DepthMap.sample(s_Material_DepthMap, in.v_TextureCoord).r;
    
    // Linearize the depth value using the near and far planes
    // NOTE:
    // - For orthographic projection, depth is already linear, so we can simply use:
    //       color = float4(float3(depthValue), 1.0f);
    // - For perspective projection, depth is non-linear, so use the linearizeDepth function:
    //       color = float4(float3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0f);
    bool linearize = (u_Material.Linearize != 0u);
    float4 color = float4(0.0f);
    if (linearize)
        color = float4(float3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0f);
    else
        color = float4(float3(depthValue), 1.0f);
    return color;
}
