#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/NormalMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/PosNorm.vs.metal"

// Include material properties
#import "pixc/shaders/shared/structure/material/PhongColorMaterial.metal"
#import "pixc/shaders/shared/structure/view/SimpleView.metal"
#import "pixc/shaders/shared/structure/light/SimpleLight.metal"
#import "pixc/shaders/shared/structure/environment/Environment.metal"

// Include additional functions
#import "pixc/shaders/shared/utils/Attenuation.metal"

#import "pixc/shaders/phong/chunks/PhongSpecular.metal"
#import "pixc/shaders/phong/chunks/Phong.metal"

// Entry point of the fragment shader
fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              constant Material &u_Material [[ buffer(BufferIndex::MaterialBuffer) ]],
                              constant View &u_View [[ buffer(BufferIndex::ViewBuffer) ]],
                              constant Environment &u_Environment [[ buffer(BufferIndex::EnvironmentBuffer) ]])
{
    // Define the initial reflectance
    float3 reflectance = float3(0.0f);
    // Shade based on each light source in the scene
    for(int i = 0; i < u_Environment.LightCount; i++)
    {
        // Define the initial reflectance
        reflectance += calculateColor(in.v_Position, in.v_Normal, u_View.Position,
                                      u_Environment.Lights[i].Vector, u_Environment.Lights[i].Color,
                                      u_Material.Kd * u_Environment.Lights[i].Ld, u_Material.Ks * u_Environment.Lights[i].Ls,
                                      u_Material.Shininess, 0.0f, 0.045f, 0.0075f, 0.7f);
    }
    
    // Calculate the ambient light
    float3 ambient = u_Environment.La * u_Material.Ka;
    
    // Set the fragment color with the calculated result and material's alpha
    float3 result = reflectance + ambient;
    float4 color = float4(result, u_Material.Alpha);
    
    return color;
}
