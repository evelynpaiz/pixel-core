#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/texture/TextureIndex.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/NormalMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/PosTexNorm.vs.metal"

// Include material properties
#import "pixc/shaders/shared/structure/material/PhongMaterial.metal"
#import "pixc/shaders/shared/structure/view/SimpleView.metal"
#import "pixc/shaders/shared/structure/light/SimpleLight.metal"
#import "pixc/shaders/shared/structure/environment/Environment.metal"

// Include additional functions
#import "pixc/shaders/shared/utils/Attenuation.metal"

#import "pixc/shaders/forward/lit/phong/chunks/PhongSpecular.metal"
#import "pixc/shaders/forward/lit/phong/chunks/Phong.metal"

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              constant Material &u_Material [[ buffer(BufferIndex::MaterialBuffer) ]],
                              constant View &u_View [[ buffer(BufferIndex::ViewBuffer) ]],
                              constant Environment &u_Environment [[ buffer(BufferIndex::EnvironmentBuffer) ]],
                              texture2d<float> u_Material_DiffuseMap [[ texture(TextureIndex::DiffuseMap) ]],
                              sampler s_Material_DiffuseMap [[ sampler(TextureIndex::DiffuseMap) ]],
                              texture2d<float> u_Material_SpecularMap [[ texture(TextureIndex::SpecularMap) ]],
                              sampler s_Material_SpecularMap [[ sampler(TextureIndex::SpecularMap) ]])
{
    // Sample texture(s)
    float3 texDiffuse = u_Material_DiffuseMap.sample(s_Material_DiffuseMap, in.v_TextureCoord).rgb;
    float3 texSpecular = u_Material_SpecularMap.sample(s_Material_SpecularMap, in.v_TextureCoord).rgb;
    
    // Combine material colors with textures
    ShadingMaterial material = {
        .Kd = u_Material.Kd * texDiffuse,
        .Ks = u_Material.Ks * texSpecular,
        .Shininess = u_Material.Shininess,
        .Flags = u_Material.Flags
    };
    
    // Define the initial reflectance
    float3 reflectance = float3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightCount; i++)
    {
        // Scale material by light intensity
        material.Kd *= u_Environment.Lights[i].Ld;
        material.Ks *= u_Environment.Lights[i].Ls;
        
        // Construct light for shading
        ShadingLight light = {
            .Vector = u_Environment.Lights[i].Vector,
            .Color = u_Environment.Lights[i].Color,
            .Linear = 0.045f,
            .Quadratic = 0.0075f,
            .DirectionalAttenuation = 0.7f,
            .ShadowFactor = 0.0f
        };
        
        // Define the initial reflectance
        reflectance += calculateColor(in.v_Position, in.v_Normal, u_View.Position,
                                      material, light);
    }
    
    // Calculate the ambient light
    float3 ambient = u_Environment.La * u_Material.Ka * texDiffuse;
    
    // Set the fragment color with the calculated result and material's alpha
    float3 result = reflectance + ambient;
    float4 color = float4(result, u_Material.Alpha);
    
    return color;
}
