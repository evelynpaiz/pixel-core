// Input vertex attributes
layout (location = 0) in vec4 a_Position;       // Vertex position in object space
layout (location = 1) in vec2 a_TextureCoord;  // Texture coordinates
layout (location = 2) in vec3 a_Normal;        // Vertex normal in object space

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

// Output to fragment shader
out vec3 v_Position;           // Vertex position in world space
out vec2 v_TextureCoord;       // Texture coordinates
out vec3 v_Normal;             // Vertex normal in world space

// Entry point of the vertex shader
void main()
{
    // Transform the vertex position and normal from object space to world space
    vec4 worldPosition = u_Transform.Model * a_Position;
    vec3 worldNormal = normalize(u_Transform.Normal * a_Normal);
    
    // Calculate the vertex position in world space
    v_Position = worldPosition.xyz / worldPosition.w;
    // Pass the input texture coordinates to the fragment shader
    v_TextureCoord = a_TextureCoord;
    // Transform the vertex normal from object space to world space
    v_Normal = worldNormal;
    
    // Calculate the final position of the vertex in clip space
    gl_Position = u_Transform.Projection * u_Transform.View * worldPosition;
}

