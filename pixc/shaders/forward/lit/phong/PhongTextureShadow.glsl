#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/common/light/CompleteLight.glsl"
#include "Resources/shaders/common/environment/Environment.glsl"
#include "Resources/shaders/common/matrix/CompleteMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/common/vertex/PTN-S.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "Resources/shaders/common/material/PhongTextureMaterial.glsl"
#include "Resources/shaders/common/view/SimpleView.glsl"
#include "Resources/shaders/common/light/CompleteLight.glsl"
#include "Resources/shaders/common/environment/Environment.glsl"

// Include fragment inputs
#include "Resources/shaders/common/fragment/PTN.fs.glsl"
#include "Resources/shaders/common/fragment/L.fs.glsl"

// Include additional functions
#include "Resources/shaders/common/utils/Saturate.glsl"
#include "Resources/shaders/common/utils/Attenuation.glsl"

#include "Resources/shaders/forward/lit/phong/chunks/PhongSpecular.glsl"
#include "Resources/shaders/forward/lit/phong/chunks/Phong.glsl"

#include "Resources/shaders/depth/chunks/PCF.glsl"
#include "Resources/shaders/depth/chunks/BiasAngle.glsl"
#include "Resources/shaders/depth/chunks/ShadowMap.glsl"

#include "Resources/shaders/environment/chunks/SHIrradiance.glsl"

///< Mathematical constants.
const float PI = 3.14159265359f;
const float INV_PI = 1.0f / PI;

// Entry point of the fragment shader
void main()
{
    // Calculate the normalized surface normal
    vec3 normal = normalize(v_Normal);

    // Get the diffuse color (kd) from the DiffuseMap texture
    vec3 kd = vec3(texture(u_Material.DiffuseMap, v_TextureCoord));
    // Get the specular color (ks) from the SpecularMap texture
    vec3 ks = vec3(texture(u_Material.SpecularMap, v_TextureCoord));
    
    // Define the initial reflectance
    vec3 reflectance = vec3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightsNumber; i++)
    {
        // Calculate the normalized light direction vector
        vec3 lightDirection = u_Light[i].Vector.w == 1.0f ?
                              normalize(u_Light[i].Vector.xyz - v_Position) :   // positional light (.w = 1)
                              normalize(-u_Light[i].Vector.xyz);                // directional light (.w = 0)
        
        // Calculate shadow factor
        float bias = calculateBias(normal, lightDirection, 0.005f, 0.01f);
        float shadow = calculateShadow(u_Light[i].ShadowMap, v_LightSpacePosition[i], bias, 11, 1.0f);
        
        // Define fragment color using Phong shading
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position, u_Light[i].Vector,
                                      u_Light[i].Color, kd * u_Light[i].Ld, ks * u_Light[i].Ls,
                                      u_Material.Shininess, shadow, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    vec3 irradiance = calculateIrradiance(u_Environment.IrradianceMatrix, normal, INV_PI);
    vec3 ambient = irradiance * u_Environment.La * kd;
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
