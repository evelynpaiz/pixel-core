#shader vertex
#version 330 core

// Include transformation matrices
#include "Resources/shaders/shared/structure/matrix/SimpleMatrix.glsl"

// Input vertex attributes
layout (location = 0) in vec4 a_Position;   ///< Vertex position in object space

// Uniform buffer block containing transformation matrices
uniform Transform u_Transform;

// Outputs to fragment shader
out vec3 v_Position;                        ///< Vertex position in world space

// Entry point of the vertex shader
void main()
{
    // Transform the vertex position and normal from object space to world space
    vec4 worldPosition = u_Transform.Model * a_Position;
    v_Position = worldPosition.xyz;
    
    // Calculate the final position of the vertex in clip space
    gl_Position = u_Transform.Projection * u_Transform.View * worldPosition;
}

#shader fragment
#version 330 core

/**
 * Represents the material properties of an object.
 */
struct Material {
    samplerCube TextureMap;                 ///< Texture map applied to the material.
};

// Specify the output color of the fragment shader
layout (location = 0) out vec4 color;

// Uniform buffer blocks
uniform Material u_Material;                ///< Material properties

// Input variables from the vertex shader
in vec3 v_Position;                         ///< Vertex position in world space

// Define constant variables
const float PI = 3.14159265359f;

// Entry point of the fragment shader
void main()
{
    // Calculate the normal vector from the vertex position in world space
    vec3 normal = normalize(v_Position);
    
    // Initialize the irradiance value
    vec3 irradiance = vec3(0.0f);
    
    // Calculate the tangent space at the origin point
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    
    // Define sampling properties
    float delta = 0.025f;  // Sampling step size
    float samples = 0.0f;  // Number of samples taken
    
    // Iterate over phi (azimuthal angle) and theta (polar angle)
    for(float phi = 0.0f; phi < 2.0f * PI; phi += delta)
    {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += delta)
        {
            // Convert spherical coordinates to tangent space (in the hemisphere)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            
            // Convert tangent space vector to world space
            vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            // Sample the environment map and accumulate the irradiance
            irradiance += texture(u_Material.TextureMap, sampleVector).rgb * cos(theta) * sin(theta);
            samples++;
        }
    }
    
    // Normalize and scale the irradiance
    irradiance = PI * irradiance * (1.0 / float(samples));
    
    // Set the output color to the irradiance
    color = vec4(irradiance, 1.0f);
}
