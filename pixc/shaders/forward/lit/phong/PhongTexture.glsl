#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/NormalMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/PosTexNorm.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "pixc/shaders/shared/structure/material/PhongTextureMaterial.glsl"
#include "pixc/shaders/shared/structure/view/SimpleView.glsl"
#include "pixc/shaders/shared/structure/light/SimpleLight.glsl"
#include "pixc/shaders/shared/structure/environment/Environment.glsl"

// Include fragment inputs
#include "pixc/shaders/shared/chunk/fragment/PosTexNorm.fs.glsl"

// Include additional functions
#include "pixc/shaders/shared/utils/Saturate.glsl"
#include "pixc/shaders/shared/utils/Attenuation.glsl"

#include "pixc/shaders/forward/lit/phong/chunks/PhongSpecular.glsl"
#include "pixc/shaders/forward/lit/phong/chunks/Phong.glsl"

///< Mathematical constants.
const float PI = 3.14159265359f;
const float INV_PI = 1.0f / PI;

// Entry point of the fragment shader
void main()
{
    // Get the diffuse color (kd) from the DiffuseMap texture
    vec3 kd = vec3(texture(u_Material.DiffuseMap, v_TextureCoord));
    // Get the specular color (ks) from the SpecularMap texture
    vec3 ks = vec3(texture(u_Material.SpecularMap, v_TextureCoord));
    
    // Define the initial reflectance
    vec3 reflectance = vec3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightCount; i++)
    {
        // Define fragment color using Phong shading
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position,
                                      u_Environment.Lights[i].Vector, u_Environment.Lights[i].Color,
                                      kd * u_Environment.Lights[i].Ld, ks * u_Environment.Lights[i].Ls,
                                      u_Material.Shininess, 0.0f, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    vec3 ambient = u_Environment.La * kd;   // NOTE: using ka as the diffuse map
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
