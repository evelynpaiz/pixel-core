#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/Base.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "pixc/shaders/shared/structure/material/ColorMaterial.glsl"

// Include fragment inputs
#include "pixc/shaders/shared/chunk/fragment/Base.fs.glsl"

// Entry point of the fragment shader
void main()
{
    // Set the output color of the fragment shader to the input material color
    color = u_Material.Color;
}
