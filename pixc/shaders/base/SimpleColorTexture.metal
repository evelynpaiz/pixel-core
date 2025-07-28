#include <metal_stdlib>
using namespace metal;

// Include indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"
#import "pixc/shaders/shared/enum/material/TextureMaterial.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/Tex.vs.metal"

// Include material properties
#import "pixc/shaders/shared/structure/material/ColorMaterial.metal"

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
