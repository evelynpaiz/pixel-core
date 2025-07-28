#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/common/matrix/SimpleMatrix.glsl"

// Include vertex shader
#include "Resources/shaders/common/vertex/PT.vs.glsl"

#shader fragment
#version 330 core

// Include material properties
#include "Resources/shaders/common/material/TextureMaterial.glsl"

// Include fragment inputs
layout (location = 0) out float outColor;

uniform Material u_Material;

in vec2 v_TextureCoord;

// Entry point of the fragment shader
void main()
{
    vec2 texelSize = 1.0f / vec2(textureSize(u_Material.TextureMap, 0));
    float result = 0.0f;

    const int kernelSize = 5;
    const int halfKernel = kernelSize / 2;

    for (int x = -halfKernel; x <= halfKernel; ++x)
    {
        for (int y = -halfKernel; y <= halfKernel; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(u_Material.TextureMap, v_TextureCoord + offset).r;
        }
    }
    
    // Divide by the number of samples (kernelSize * kernelSize)
    outColor = result / float(kernelSize * kernelSize);
}

