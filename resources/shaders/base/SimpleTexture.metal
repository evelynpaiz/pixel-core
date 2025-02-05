#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "Resources/shaders/shared/enum/buffer/Buffer.metal"
#import "Resources/shaders/shared/enum/material/TextureMaterial.metal"

// Include transformation matrices
#import "Resources/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "Resources/shaders/shared/chunk/vertex/Tex.vs.metal"

fragment float4 fragment_main(const VertexOut in [[ stage_in ]],
                              texture2d<float> u_Material_TextureMap [[ texture(MaterialIndex::TextureMap) ]],
                              sampler s_Material_TextureMap [[ sampler(MaterialIndex::TextureMap) ]])
{
    // Sample the color from the texture using the provided texture coordinates
    float4 color = u_Material_TextureMap.sample(s_Material_TextureMap, in.v_TextureCoord);
    
    return color;
}
