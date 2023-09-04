// Input vertex attributes
layout (location = 0) in vec4 a_Position; // Vertex position in object space
layout (location = 1) in vec3 a_Normal;   // Vertex normal in object space

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

// Outputs to fragment shader
out vec3 v_Position;             // Vertex position in world space
out vec3 v_Normal;               // Vertex normal in world space
out vec4 v_LightSpacePosition;   // Vertex position in light space

// Entry point of the vertex shader
void main()
{
    // Transform the vertex position and normal from object space to world space
    vec4 worldPosition = u_Transform.Model * a_Position;
    vec3 worldNormal = normalize(u_Transform.Normal * a_Normal);

    // Transform the vertex position from world space to light space
    vec4 lightSpacePosition = u_Transform.Texture * u_Transform.Light * worldPosition;

    // Perspective divide to get vertex position in normalized device coordinates
    v_Position = worldPosition.xyz / worldPosition.w;

    // Pass the transformed normal to the fragment shader
    v_Normal = worldNormal;

    // Pass the vertex position in light space to the fragment shader
    v_LightSpacePosition = lightSpacePosition;

    // Calculate the final position of the vertex in clip space
    gl_Position = u_Transform.Projection * u_Transform.View * worldPosition;
}
