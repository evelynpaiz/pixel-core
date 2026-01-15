#include "pixcpch.h"
#include "Foundation/Renderer/Light/Environment/SHEnvironmentLight.h"

#include "Foundation/Renderer/Texture/TextureCube.h"
#include "Foundation/Renderer/Material/SimpleMaterial.h"

#include "Foundation/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"
#include "Foundation/Renderer/Utils/CubeMapUtils.h"

namespace pixc {

/**
 * @brief Construct an environment ligth in the scene.
 *
 * @param size The resolution of the cubemap (width = height).
 */
SHEnvironmentLight::SHEnvironmentLight(const uint32_t size)
{
    // Initialize the environment light
    InitEnvironment(size);
}

/**
 * Initialize framebuffers for the environment light.
 *
 * @param size The resolution of the cubemap (width = height).
 */
void SHEnvironmentLight::SetupFrameBuffers(const uint32_t size)
{
    // Create a framebuffer specification for storing SH coefficients
    FrameBufferSpecification spec;
    spec.SetFrameBufferSize(3, 3);  // small 3x3 framebuffer for SH computation
    spec.MipMaps = false;  // mipmaps are unnecessary for SH coefficient storage
    
    // Define a texture specification for the SH framebuffer
    TextureSpecification sh(TextureType::TEXTURE2D, TextureFormat::RGB16F);
    sh.SetMinMagFilter(TextureFilter::Nearest);  // no interpolation; precise sampling
    // Attach the texture to the framebuffer specification
    spec.AttachmentsSpec = { { sh } };
    
    // Create the framebuffer in the library
    m_FrameBuffers.Create("SphericalHarmonics", spec);
}

/**
 * @brief Setup the resources for the environment light.
 */
void SHEnvironmentLight::SetupResources()
{
    // Get the material library defined in the renderer
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    
    // Spherical harmonics
    if (!materialLibrary.Exists("SphericalHarmonics"))
    {
        materialLibrary.Create<TextureMaterial>("SphericalHarmonics",
                                                "pixc/shaders/environment/sh/SphericalHarmonics");
    }
}

/**
 * @brief Define light properties into the uniforms of the shader program.
 *
 * @param shader The shader program.
 * @param properties The flags indicating which light properties should be defined.
 * @param slot The texture slot to bind the environment map to.
 */
void SHEnvironmentLight::DefineLightProperties(const std::shared_ptr<Shader> &shader,
                                               LightProperty properties)
{
    EnvironmentLight::DefineLightProperties(shader, properties);
    
    // Using texture slot 0 for the environment map
    if (m_EnvironmentMap)
    {
        auto environment = m_FrameBuffers.Get("Environment")->GetColorAttachment(0);
        shader->SetTexture("u_Environment.EnvironmentMap", environment,
                           static_cast<uint32_t>(TextureIndex::EnvironmentMap));
    }
    else
    {
        static auto &white = pixc::utils::textures::WhiteTextureCube();
        shader->SetTexture("u_Environment.EnvironmentMap", white,
                           static_cast<uint32_t>(TextureIndex::EnvironmentMap));
    }
    
    // Define the irradiance information using spherical harmonics
    auto matrix = HasProperty(properties, LightProperty::DirectionDependent) ?
                  m_Coefficients.m_Anisotropic : m_Coefficients.m_Isotropic;
    shader->SetMat4("u_Environment.Irradiance.Red", matrix.Red);
    shader->SetMat4("u_Environment.Irradiance.Green", matrix.Green);
    shader->SetMat4("u_Environment.Irradiance.Blue", matrix.Blue);
}

/**
 * Updates the environment lighting information.
 */
void SHEnvironmentLight::UpdateEnvironment()
{
    // Call base function related to the general environment light
    EnvironmentLight::UpdateEnvironment();
    
    // Get the material library defined in the renderer
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    // Update the current texture representing the environment map
    auto material = std::dynamic_pointer_cast<TextureMaterial>(materialLibrary.Get("SphericalHarmonics"));
    material->SetTextureMap(m_FrameBuffers.Get("Environment")->GetColorAttachment(0));
    
    // Create a plane geometry (to render to) using the material
    using VertexData = GeoVertexData<glm::vec4>;
    auto geometry = utils::geometry::ModelPlane<VertexData>(material);
    geometry->SetScale(glm::vec3(2.0f));
    
    // Get the spherical harmonics framebuffer
    auto& framebuffer = m_FrameBuffers.Get("SphericalHarmonics");
    
    // Render the scene to compute the SH coefficients
    RendererCommand::BeginRenderPass(framebuffer);
    RendererCommand::SetClearColor(glm::vec4(0.0f));
    RendererCommand::Clear();
    
    Renderer::BeginScene();
    geometry->SetMaterial(material);
    geometry->DrawModel();
    Renderer::EndScene();
    
    RendererCommand::EndRenderPass();
    GraphicsContext::Get().SwapBuffers();
    
    // Retrieve the information of the SH coefficients
    std::vector<char> rawData = framebuffer->GetAttachmentData(0);
    // Ensure the raw data size is compatible with half float conversion
    PIXEL_CORE_ASSERT(rawData.size() % sizeof(uint16_t) == 0, "Data size is not a multiple of float size!");
    
    // Treat raw data as half-floats
    size_t halfCount = rawData.size() / sizeof(uint16_t);
    const uint16_t* halfData = reinterpret_cast<const uint16_t*>(rawData.data());
    // Determine whether alpha channel is present
    bool hasAlpha = (halfCount % 4) == 0;
    // Convert half floats → full floats
    std::vector<float> data;
    for (size_t i = 0; i < halfCount; ++i)
    {
        if (!hasAlpha || (i % 4) != 3)
            data.push_back(glm::detail::toFloat32(halfData[i]));
    }

    // Compute the SH matrices from the coefficents
    m_Coefficients.UpdateFromCoefficients(data);
}

/**
 * Updates the SH matrices from spherical harmonic coefficients.
 *
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
 */
void SHCoefficients::UpdateFromCoefficients(const std::vector<float> &shCoeffs)
{
    // Isotropic matrix
    m_Isotropic.Red = GenerateIsotropicMatrix(0, shCoeffs);
    m_Isotropic.Green = GenerateIsotropicMatrix(1, shCoeffs);
    m_Isotropic.Blue = GenerateIsotropicMatrix(2, shCoeffs);
    
    // Anisotropic matrix
    m_Anisotropic.Red = GenerateAnisotropicMatrix(0, shCoeffs);
    m_Anisotropic.Green = GenerateAnisotropicMatrix(1, shCoeffs);
    m_Anisotropic.Blue = GenerateAnisotropicMatrix(2, shCoeffs);
}

/**
 * Generates the M matrix for isotropic irradiance calculation for a specific color channel.
 *
 * This function constructs the 4x4 matrix M, which is used for efficient irradiance
 * calculations for isotropic (Lambertian) surfaces using spherical harmonics. The matrix is constructed
 * based on the formulas presented in the paper "An Efficient Representation for Irradiance Environment Maps."
 *
 * @param colorIndex The index of the color channel (0 = red, 1 = green, 2 = blue).
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
 *
 * @return The 4x4 matrix M for the specified color channel.
 */
glm::mat4 SHCoefficients::GenerateIsotropicMatrix(int colorIndex,
                                                  const std::vector<float> &shCoeffs)
{
    static constexpr float c1 = 0.429043f;
    static constexpr float c2 = 0.511664f;
    static constexpr float c3 = 0.743125f;
    static constexpr float c4 = 0.886227f;
    static constexpr float c5 = 0.247708f;
    
    glm::mat4 M(0.0f); // Initialize M to zero
    
    // Construct M using the precomputed L coefficients
    M[0][0] = c1 * shCoeffs[8 * 3 + colorIndex];    // L22
    M[0][1] = c1 * shCoeffs[4 * 3 + colorIndex];    // L2-2
    M[0][2] = c1 * shCoeffs[7 * 3 + colorIndex];    // L21
    M[0][3] = c2 * shCoeffs[3 * 3 + colorIndex];    // L11
    
    M[1][0] = M[0][1];                              // Symmetry
    M[1][1] = -M[0][0];                             // Symmetry
    M[1][2] = c1 * shCoeffs[5 * 3 + colorIndex];    // L2-1
    M[1][3] = c2 * shCoeffs[1 * 3 + colorIndex];    // L1-1
    
    M[2][0] = M[0][2];                              // Symmetry
    M[2][1] = M[1][2];                              // Symmetry
    M[2][2] = c3 * shCoeffs[6 * 3 + colorIndex];    // L20
    M[2][3] = c2 * shCoeffs[2 * 3 + colorIndex];    // L10
    
    M[3][0] = M[0][3];                              // Symmetry
    M[3][1] = M[1][3];                              // Symmetry
    M[3][2] = M[2][3];                              // Symmetry
    M[3][3] = c4 * shCoeffs[0 * 3 + colorIndex]
    - c5 * shCoeffs[6 * 3 + colorIndex];            // L00 and L20
    
    return M;
}

/**
 * Generates the M matrix for anisotropic irradiance calculation for a specific color channel.
 *
 * This function constructs the 4x4 matrix M, which is used for efficient irradiance
 * calculations for anisotropic surfaces (using surface tangents) using spherical harmonics.
 * The matrix is constructed based on the formulas presented in the paper
 * "Analytic Tangent Irradiance Environment Maps for Anisotropic Surfaces".
 *
 * @param colorIndex The index of the color channel (0 = red, 1 = green, 2 = blue).
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
 *
 * @return The 4x4 matrix M for the specified color channel.
 */
glm::mat4 SHCoefficients::GenerateAnisotropicMatrix(int colorIndex,
                                                    const std::vector<float> &shCoeffs)
{
    static constexpr float b0 =  0.282095f;
    static constexpr float b1 = -0.0682843f;
    static constexpr float b2 = -0.118272f;
    static constexpr float b3 =  0.0394239f;
    
    glm::mat4 M(0.0f); // Initialize M to zero
    
    // Construct M using the precomputed L coefficients
    M[0][0] = b1 * shCoeffs[8 * 3 + colorIndex];    // L22
    M[0][1] = b1 * shCoeffs[4 * 3 + colorIndex];    // L2-2
    M[0][2] = b1 * shCoeffs[7 * 3 + colorIndex];    // L21
    M[0][3] = 0.0f;                                 // L11
    
    M[1][0] = M[0][1];                              // Symmetry
    M[1][1] = -M[0][0];                             // Symmetry
    M[1][2] = b1 * shCoeffs[5 * 3 + colorIndex];    // L2-1
    M[1][3] = 0.0f;                                 // L1-1
    
    M[2][0] = M[0][2];                              // Symmetry
    M[2][1] = M[1][2];                              // Symmetry
    M[2][2] = b2 * shCoeffs[6 * 3 + colorIndex];    // L20
    M[2][3] = 0.0f;                                 // L10
    
    M[3][0] = M[0][3];                              // Symmetry
    M[3][1] = M[1][3];                              // Symmetry
    M[3][2] = M[2][3];                              // Symmetry
    M[3][3] = b0 * shCoeffs[0 * 3 + colorIndex]
    + b3 * shCoeffs[6 * 3 + colorIndex];    // L00 and L20
    
    return M;
}

} // namespace pixc
