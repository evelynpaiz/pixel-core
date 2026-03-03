#shader vertex
#version 330 core

// Include transformation matrices
#include "pixc/shaders/shared/structure/matrix/NormalMatrix.glsl"

// Include vertex shader
#include "pixc/shaders/shared/chunk/vertex/PosTexNorm.vs.glsl"

#shader fragment
#version 330 core

// Include material, view and light properties
#include "pixc/shaders/shared/structure/material/PhongMaterial.glsl"
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
    // Sample texture(s)
    vec3 texDiffuse  = texture(u_Material.DiffuseMap,  v_TextureCoord).rgb;
    vec3 texSpecular = texture(u_Material.SpecularMap, v_TextureCoord).rgb;

    // Combine material colors with textures
    ShadingMaterial material = ShadingMaterial(
        u_Material.Kd * texDiffuse,   // Kd
        u_Material.Ks * texSpecular,  // Ks
        u_Material.Shininess,         // Shininess
        u_Material.Flags              // Flags
    );
    
    // Define the initial reflectance
    vec3 reflectance = vec3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightCount; i++)
    {
        // Scale material by light intensity
        material.Kd *= u_Environment.Lights[i].Ld;
        material.Ks *= u_Environment.Lights[i].Ls;
        
        // Construct light for shading
        ShadingLight light = ShadingLight(
            u_Environment.Lights[i].Vector,
            u_Environment.Lights[i].Color,
            0.045f,   // Linear attenuation
            0.0075f,  // Quadratic attenuation
            0.7f,     // Directional attenuation
            0.0f      // Shadow factor
        );
        
        // Calculate the shading result using Phong shading model
        reflectance += calculateColor(v_Position, v_Normal, u_View.Position, material, light);
    }
    
    // Calculate the ambient light
    vec3 ambient = u_Environment.La * u_Material.Ka * texDiffuse;
    
    // Set the fragment color with the calculated result and material's alpha
    vec3 result = reflectance + ambient;
    color = vec4(result, u_Material.Alpha);
}
