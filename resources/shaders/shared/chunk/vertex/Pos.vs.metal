/**
 * Represents the layout of incoming vertex data.
 */
struct VertexIn
{
    float4 a_Position [[ attribute(0) ]];     ///< Vertex position in model space.
};

/**
 * Represents the layout of the outcome data for the fragment shader.
 */
struct VertexOut
{
    float4 Position [[position]];
    float3 v_Position;
};

// Entry point of the vertex shader
vertex VertexOut vertex_main(const VertexIn in [[ stage_in ]],
                             constant Transform &u_Transform [[ buffer(BufferIndex::Transformations) ]])
{
    // Transform the vertex position from object space to world space
    float4 worldPosition = u_Transform.Model * in.a_Position;
    
    // Calculate the final position of the vertex in clip space
    // by transforming the vertex position from object space to clip space
    float4 position = u_Transform.Projection * u_Transform.View * worldPosition;
    
    // Pass the input attributes to the fragment shader
    VertexOut out {
        .Position = position,
        .v_Position = worldPosition.xyz
    };
    return out;
}
