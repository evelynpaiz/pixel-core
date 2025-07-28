#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "Resources/shaders/shared/enum/buffer/Buffer.metal"

// Include transformation matrices
#import "Resources/shaders/shared/structure/matrix/NormalMatrix.metal"

// Include vertex shader
#import "Resources/shaders/shared/chunk/vertex/PosNorm.vs.metal"

// Include material properties
#import "Resources/shaders/shared/structure/material/PhongColorMaterial.metal"
#import "Resources/shaders/shared/structure/view/SimpleView.metal"
#import "Resources/shaders/shared/structure/light/SimpleLight.metal"
#import "Resources/shaders/shared/structure/light/EnvironmentLight.metal"

fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              constant Material &u_Material [[ buffer(BufferIndex::Materials) ]],
                              texture2d<float> u_Material_TextureMap [[ texture(MaterialIndex::TextureMap) ]],
                              sampler s_Material_TextureMap [[ sampler(MaterialIndex::TextureMap) ]])
{
    // Sample the color from the texture using the provided texture coordinates
    float4 textureColor = u_Material_TextureMap.sample(s_Material_TextureMap, in.v_TextureCoord);
    
    // Combine the sampled texture color with the material color
    float4 color = textureColor * u_Material.Color;
    return color;
}
