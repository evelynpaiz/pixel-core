// Specify the output color of the fragment shader
layout (location = 0) out vec4 color;

// Uniform buffer blocks
uniform Material u_Material;    // Material properties

// Input variables from the vertex shader
in vec2 v_TextureCoord;         // Texture coordinates
