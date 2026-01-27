// Input vertex attributes
layout (location = 0) in vec4 a_Position;       // Vertex position in model space
layout (location = 1) in vec2 a_TextureCoord;   // Texture coordinates

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

// Output to fragment shader
out vec2 v_TextureCoord;                        // Pass texture coordinates to the fragment shader

// Entry point of the vertex shader
void main()
{
    // Pass the input texture coordinates to the fragment shader
    v_TextureCoord = a_TextureCoord;
    
    // Calculate the final position of the vertex in clip space
    // by transforming the vertex position from object space to clip space
    gl_Position = u_Transform.Projection * u_Transform.View * u_Transform.Model * a_Position;
    // Convert [0,1] depth to OpenGL [-1,1]
    gl_Position.z = gl_Position.z * 2.0 - gl_Position.w;
}
