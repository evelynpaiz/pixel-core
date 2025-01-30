#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/shared/structure/matrix/NormalMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/shared/chunk/vertex/PosNorm.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "Resources/shaders/shared/structure/material/PhongColorMaterial.glsl"
#include "Resources/shaders/shared/structure/view/SimpleView.glsl"
#include "Resources/shaders/shared/structure/light/SimpleLight.glsl"
#include "Resources/shaders/shared/structure/light/EnvironmentLight.glsl"

// Include fragment inputs
#include "Resources/shaders/shared/chunk/fragment/PosNorm.fs.glsl"

// Include additional functions
#include "Resources/shaders/shared/utils/Saturate.glsl"
#include "Resources/shaders/shared/utils/Attenuation.glsl"

#include "Resources/shaders/phong/chunks/PhongSpecular.glsl"
#include "Resources/shaders/phong/chunks/Phong.glsl"

#include "Resources/shaders/environment/chunks/SHIrradiance.glsl"

///< Mathematical constants.
const float PI = 3.14159265359f;
const float INV_PI = 1.0f / PI;

// Entry point of the fragment shader
void main()
{
    // Define the initial reflectance
    vec3 reflectance = vec3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightsNumber; i++)
    {
        // Calculate the shading result using Phong shading model
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position, u_Light[i].Vector, u_Light[i].Color,
                                      u_Material.Kd * u_Light[i].Ld, u_Material.Ks * u_Light[i].Ls,
                                      u_Material.Shininess, 0.0f, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    vec3 irradiance = calculateIrradiance(u_Environment.IrradianceMatrix, normal, INV_PI);
    vec3 ambient = irradiance * u_Environment.La * u_Material.Ka;
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
