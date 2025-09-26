#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/common/matrix/NormalMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/common/vertex/PTN.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "Resources/shaders/common/material/PhongTextureMaterial.glsl"
#include "Resources/shaders/common/view/SimpleView.glsl"
#include "Resources/shaders/common/light/SimpleLight.glsl"
#include "Resources/shaders/common/environment/Environment.glsl"

// Include fragment inputs
#include "Resources/shaders/common/fragment/PTN.fs.glsl"

// Include additional functions
#include "Resources/shaders/common/utils/Saturate.glsl"
#include "Resources/shaders/common/utils/Attenuation.glsl"

#include "Resources/shaders/phong/chunks/PhongSpecular.glsl"
#include "Resources/shaders/phong/chunks/Phong.glsl"

#include "Resources/shaders/environment/chunks/SHIrradiance.glsl"

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
    for(int i = 0; i < u_Environment.LightsNumber; i++)
    {
        // Define fragment color using Phong shading
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position, u_Light[i].Vector,
                                      u_Light[i].Color, kd * u_Light[i].Ld, ks * u_Light[i].Ls,
                                      u_Material.Shininess, 0.0f, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    vec3 irradiance = calculateIrradiance(u_Environment.IrradianceMatrix, normal, INV_PI);
    vec3 ambient = irradiance * u_Environment.La * kd;
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
