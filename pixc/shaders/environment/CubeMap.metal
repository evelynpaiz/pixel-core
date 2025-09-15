#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/material/TextureMaterial.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

/**
 * Represents the layout of incoming vertex data.
 */
struct VertexIn
{
    float4 a_Position [[ attribute(0) ]];       ///< Vertex position in model space.
};

/**
 * Represents the layout of the outcome data for the fragment shader.
 */
struct VertexOut
{
    float4 Position [[position]];               ///< Final vertex position in clip space.
    
    float3 v_Position;                          ///< Vertex position in world space.
};

// Entry point of the vertex shader
vertex VertexOut vertex_main(const VertexIn in [[ stage_in ]],
                             constant Transform &u_Transform [[ buffer(BufferIndex::TransformBuffer) ]])
{
    // Transform the vertex position from object space to world space
    float4 worldPosition = u_Transform.Model * in.a_Position;
    
    // Remove translation from the view matrix (compact version)
    float4x4 view = float4x4(
        float4(u_Transform.View[0].xyz, 0.0),
        float4(u_Transform.View[1].xyz, 0.0),
        float4(u_Transform.View[2].xyz, 0.0),
        float4(0.0, 0.0, 0.0, 1.0)
    );
    // Calculate the final position of the vertex in clip space
    float4 clipPosition = u_Transform.Projection * view * worldPosition;
    // Manually construct xyww
    float4 position = float4(clipPosition.x, clipPosition.y, clipPosition.w, clipPosition.w);
    
    // Pass the input attributes to the fragment shader
    VertexOut out {
        .Position = clipPosition,   // TODO: use position instead when depth function incorporated
        .v_Position = worldPosition.xyz
    };
    return out;
}

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              texturecube<float> u_Material_TextureMap [[ texture(MaterialIndex::TextureMap) ]],
                              sampler s_Material_TextureMap [[ sampler(MaterialIndex::TextureMap) ]])
{
    
    float3 dir = normalize(in.v_Position);
    dir.y = -dir.y;     // flip the Y coordinate
    
    // Sample the color from the texture using the provided texture coordinates
    float4 color = u_Material_TextureMap.sample(s_Material_TextureMap, dir);
    
    return color;
}
