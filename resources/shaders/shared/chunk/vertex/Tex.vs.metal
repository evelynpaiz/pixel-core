/**
 * Represents the layout of incoming vertex data.
 */
struct VertexIn
{
    float4 a_Position [[ attribute(0) ]];     ///< Vertex position in model space.
    float2 a_TextureCoord [[ attribute(1) ]]; ///< Texture coordinates.
    
};

/**
 * Represents the layout of the outcome data for the fragment shader.
 */
struct VertexOut
{
    float4 Position [[position]];
    float2 v_TextureCoord;
};

// Entry point of the vertex shader
vertex VertexOut vertex_main(const VertexIn in [[ stage_in ]],
                             constant Transform &u_Transform [[ buffer(BufferIndex::Transformations) ]])
{
    // Calculate the final position of the vertex in clip space
    // by transforming the vertex position from object space to clip space
    float4 position = u_Transform.Projection * u_Transform.View * u_Transform.Model * in.a_Position;
    // Flip the texture coordinates
    float2 uv = float2(in.a_TextureCoord.x, 1.0f - in.a_TextureCoord.y);
    
    // Pass the input attributes to the fragment shader
    VertexOut out {
        .Position = position,
        .v_TextureCoord = uv
    };
    return out;
}
