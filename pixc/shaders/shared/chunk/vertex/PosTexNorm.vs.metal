/**
 * Represents the layout of incoming vertex data.
 */
struct VertexIn
{
    float4 a_Position [[ attribute(0) ]];       ///< Vertex position in model space.
    float2 a_TextureCoord [[ attribute(1) ]];   ///< Texture coordinates.
    float3 a_Normal [[ attribute(2) ]];         ///< Vertex normal in model space
};

/**
 * Represents the layout of the outcome data for the fragment shader.
 */
struct VertexOut
{
    float4 Position [[position]];               ///< Final vertex position in clip space.
    
    float3 v_Position;                          ///< Vertex position in world space.
    float2 v_TextureCoord;                      ///< Texture coordinate.
    float3 v_Normal;                            ///< Vertex normal in world space.
};

// Entry point of the vertex shader
vertex VertexOut vertex_main(const VertexIn in [[ stage_in ]],
                             constant Transform &u_Transform [[ buffer(BufferIndex::TransformBuffer) ]])
{
    // Transform the vertex position and normal from object space to world space
    float4 worldPosition = u_Transform.Model * in.a_Position;
    float4 worldNormal = normalize(u_Transform.Normal * float4(in.a_Normal, 0.0f));
    
    // Calculate the final position of the vertex in clip space
    // by transforming the vertex position from object space to clip space
    float4 position = u_Transform.Projection * u_Transform.View * worldPosition;
    
    // Flip the texture coordinates if necessary with:
    float2 uv = float2(in.a_TextureCoord.x, 1.0f - in.a_TextureCoord.y);
    
    // Pass the input attributes to the fragment shader
    VertexOut out {
        .Position = position,
        .v_Position = worldPosition.xyz,
        .v_TextureCoord = uv,
        .v_Normal = worldNormal.xyz
    };
    return out;
}
