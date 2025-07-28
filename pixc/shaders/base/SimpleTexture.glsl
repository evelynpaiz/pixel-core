#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/Tex.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "pixc/shaders/shared/structure/material/TextureMaterial.glsl"

// Include fragment inputs
#include "pixc/shaders/shared/chunk/fragment/Tex.fs.glsl"

// Entry point of the fragment shader
void main()
{
    // Sample the color from the texture using the provided texture coordinates
    vec4 textureColor = texture(u_Material.TextureMap, v_TextureCoord);
    // Set the output color of the fragment shader to the color from the texture
    color = textureColor;
}
