#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/NormalMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/PosNorm.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "pixc/shaders/shared/structure/material/PhongColorMaterial.glsl"
#include "pixc/shaders/shared/structure/view/SimpleView.glsl"
#include "pixc/shaders/shared/structure/light/SimpleLight.glsl"
#include "pixc/shaders/shared/structure/environment/SHEnvironment.glsl"

// Include fragment inputs
#include "pixc/shaders/shared/chunk/fragment/PosNorm.fs.glsl"

// Include additional functions
#include "pixc/shaders/shared/utils/Saturate.glsl"
#include "pixc/shaders/shared/utils/Attenuation.glsl"

#include "pixc/shaders/phong/chunks/PhongSpecular.glsl"
#include "pixc/shaders/phong/chunks/Phong.glsl"

#include "pixc/shaders/environment/chunks/SHIrradiance.glsl"
#include "pixc/shaders/environment/chunks/SHAmbientIsotropic.glsl"

// Entry point of the fragment shader
void main()
{
    // Define the initial reflectance
    vec3 reflectance = vec3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightCount; i++)
    {
        // Calculate the shading result using Phong shading model
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position,
                                      u_Environment.Lights[i].Vector, u_Environment.Lights[i].Color,
                                      u_Material.Kd * u_Environment.Lights[i].Ld, u_Material.Ks * u_Environment.Lights[i].Ls,
                                      u_Material.Shininess, 0.0f, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    vec3 ambient = calculateAmbientIsotropic(v_Position, v_Normal, u_View.Position,
                                             u_Material.Ka, u_Material.Shininess, u_Environment.La,
                                             u_Environment.EnvironmentMap, u_Environment.Irradiance);
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
