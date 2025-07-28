#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/common/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/common/vertex/PT.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "Resources/shaders/common/material/TextureMaterial.glsl"

// Include fragment inputs
#include "Resources/shaders/common/fragment/T.fs.glsl"

// Include linearization function for pespective projections
#include "Resources/shaders/depth/chunks/LinearizeDepth.glsl"

// Entry point of the fragment shader
void main()
{
    // Define the near and far planes for depth linearization
    // TODO: Pass these values as uniforms for more flexibility
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    // Sample the depth value from the texture using the provided texture coordinates
    float depthValue = texture(u_Material.TextureMap, v_TextureCoord).r;
    
    // Linearize the depth value using the near and far planes
    // For orthographic projection: Use "color = vec4(vec3(depthValue), 1.0);" instead
    // For perspective projection: Use the linearizeDepth function to linearize the depth
    // "color = vec4(vec3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0);"
    color = vec4(vec3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0);
}
