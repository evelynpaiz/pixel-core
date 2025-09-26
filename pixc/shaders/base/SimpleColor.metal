#include <metal_stdlib>
using namespace metal;

// Include buffer indices enumeration
#import "pixc/shaders/shared/enum/buffer/Buffer.metal"

// Include transformation matrices
#import "pixc/shaders/shared/structure/matrix/SimpleMatrix.metal"

// Include vertex shader
#import "pixc/shaders/shared/chunk/vertex/Base.vs.metal"

// ----------------------------------

// Include material properties
#import "pixc/shaders/shared/structure/material/ColorMaterial.metal"

// Entry point of the fragment shader
fragment float4 fragment_main(constant Material &u_Material [[ buffer(BufferIndex::MaterialBuffer) ]])
{
    // Set the output color of the fragment shader to the input material color
    return u_Material.Color;
}
