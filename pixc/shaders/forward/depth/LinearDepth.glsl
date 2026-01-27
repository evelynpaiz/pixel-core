#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/Tex.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "pixc/shaders/shared/structure/material/DepthMaterial.glsl"

// Include fragment inputs
#include "pixc/shaders/shared/chunk/fragment/Tex.fs.glsl"

// Include linearization function for pespective projections
#include "pixc/shaders/forward/depth/chunks/LinearizeDepth.glsl"

// Entry point of the fragment shader
void main()
{
    // Define the near and far planes for depth linearization
    float nearPlane = u_Material.NearPlane;
    float farPlane =  u_Material.FarPlane;

    // Sample the depth value from the texture using the provided texture coordinates
    float depthValue = texture(u_Material.DepthMap, v_TextureCoord).r;
    
    // Linearize the depth value using the near and far planes
    // NOTE:
    // - For orthographic projection, depth is already linear, so we can simply use:
    //       color = vec4(vec3(depthValue), 1.0f);
    // - For perspective projection, depth is non-linear, so use the linearizeDepth function:
    //       color = vec4(vec3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0f);
    bool linearize = (u_Material.Linearize != 0u);
    if (linearize)
        color = vec4(vec3(linearizeDepth(depthValue, nearPlane, farPlane) / farPlane), 1.0f);
    else
        color = vec4(vec3(depthValue), 1.0f);
}
