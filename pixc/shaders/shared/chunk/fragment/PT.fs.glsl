// Specify the output color of the fragment shader
layout (location = 0) out vec4 color;

// Uniform buffer blocks
uniform Material u_Material;                // Material properties

#define MAX_NUMBER_LIGHTS 4
uniform Light u_Light[MAX_NUMBER_LIGHTS];    // Light information
uniform Environment u_Environment;           // Environment properties

// Input variables from the vertex shader
in vec3 v_Position;                         // Vertex position in world space
in vec2 v_TextureCoord;                     // Texture coordinates
