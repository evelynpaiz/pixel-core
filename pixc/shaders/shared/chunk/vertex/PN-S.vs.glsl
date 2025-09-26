// Input vertex attributes
layout (location = 0) in vec4 a_Position;           // Vertex position in object space
layout (location = 1) in vec3 a_Normal;             // Vertex normal in object space

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

uniform Environment u_Environment;
#define MAX_NUMBER_LIGHTS 4
uniform Light u_Light[MAX_NUMBER_LIGHTS];

// Outputs to fragment shader
out vec3 v_Position;                                // Vertex position in world space
out vec3 v_Normal;                                  // Vertex normal in world space
out vec4 v_LightSpacePosition[MAX_NUMBER_LIGHTS];   // Vertex position in light space

// Entry point of the vertex shader
void main()
{
    // Transform the vertex position and normal from object space to world space
    vec4 worldPosition = u_Transform.Model * a_Position;
    vec3 worldNormal = normalize(u_Transform.Normal * a_Normal);

    // Pass the vertex position to the fragment shader
    v_Position = worldPosition.xyz;
    // Pass the transformed normal to the fragment shader
    v_Normal = worldNormal;
    // Pass the vertex position in light space to the fragment shader
    for(int i = 0; i < u_Environment.LightsNumber; i++)
    {
        v_LightSpacePosition[i] = u_Transform.Texture * u_Light[i].Transform * worldPosition;
    }

    // Calculate the final position of the vertex in clip space
    gl_Position = u_Transform.Projection * u_Transform.View * worldPosition;
}
