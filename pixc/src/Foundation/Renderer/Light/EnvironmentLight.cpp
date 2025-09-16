#include "pixcpch.h"
#include "Foundation/Renderer/Light/EnvironmentLight.h"

#include "Foundation/Renderer/Material/SimpleMaterial.h"

#include "Foundation/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"

#include "Foundation/Renderer/Utils/CubeMapUtils.h"

namespace pixc {

/**
 * @brief Construct an environment light source in world space.
 *
 * @param size The resolution of the cubemap (width = height).
 */
EnvironmentLight::EnvironmentLight(const unsigned int size) : Light()
{
    // Initialize the environment light
    InitEnvironment(size);
}

/**
 * @brief Setup the framebuffers for the environment light.
 *
 * @param size The resolution of the cubemap (width = height).
 */
void EnvironmentLight::InitEnvironment(unsigned int size)
{
    SetupFramebuffers(size);
    SetupResources();
}

/**
 * Initialize framebuffers for the environment light.
 *
 * @param size The resolution of the cubemap (width = height).
 */
void EnvironmentLight::SetupFramebuffers(const unsigned int size)
{
    // Define framebufer for the environment map
    FrameBufferSpecification spec;
    spec.SetFrameBufferSize(size, size);
    spec.AttachmentsSpec = {
        { TextureType::TEXTURECUBE, TextureFormat::RGB16F },
        { TextureType::TEXTURE2D, TextureFormat::DEPTH24 }
    };
    spec.MipMaps = true;
    m_Framebuffers.Create("Environment", spec);
    
    // -------
    
    //spec.SetFrameBufferSize(32, 32);
    //spec.MipMaps = false;
    //m_Framebuffers.Create("Irradiance", spec);
    
    //spec.SetFrameBufferSize(128, 128);
    //spec.MipMaps = true;
    //m_Framebuffers.Create("PreFilter", spec);
    
    // ------
    
    //spec.SetFrameBufferSize(3, 3);
    //TextureSpecification sh(TextureType::TEXTURE2D, TextureFormat::RGB16F);
    //sh.Filter = TextureFilter::Nearest;
    //spec.AttachmentsSpec = { { sh } };
    //m_Framebuffers.Create("SphericalHarmonics", spec);
}

/**
 * @brief Setup the resources for the environment light.
 */
void EnvironmentLight::SetupResources()
{
    // Get the material library defined in the renderer
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    
    // Equirectangular mapping
    if (!materialLibrary.Exists("EquirectangularMap"))
    {
        materialLibrary.Create<SimpleTextureMaterial>("EquirectangularMap",
                                                      "pixc/shaders/environment/EquirectangularMap");
    }
    // Cube mapping
    if (!materialLibrary.Exists("CubeMap"))
    {
        materialLibrary.Create<SimpleTextureMaterial>("CubeMap",
                                                      "pixc/shaders/environment/CubeMap");
    }
    
    // Define 3D model of the light source
    using VertexData = GeoVertexData<glm::vec4>;
    m_Model = utils::geometry::ModelCube<VertexData>(materialLibrary.Get("EquirectangularMap"));
    
    /*
    // Spherical harmonics
    auto sphericalHarmonics = m_Materials.Create<SimpleTextureMaterial>("SphericalHarmonics",
                                                                        "Resources/shaders/environment/SphericalHarmonicsSampling.glsl");
    sphericalHarmonics->SetTextureMap(environment);
    
    // Irradiance mapping
    auto irradiance = m_Materials.Create<SimpleTextureMaterial>("Irradiance",
                                                                "Resources/shaders/environment/IrradianceMap.glsl");
    irradiance->SetTextureMap(environment);
    
    // Pre-filtering mapping
    auto preFilter = m_Materials.Create<SimpleTextureMaterial>("PreFilter",
                                                               "Resources/shaders/environment/PreFilterMap.glsl");
    preFilter->SetTextureMap(environment);
    
    // Cube mapping
    auto cubeMap = m_Materials.Create<SimpleTextureMaterial>("Environment",
                                                             "Resources/shaders/environment/CubeMap.glsl");
    cubeMap->SetTextureMap(environment);
     */
}

/**
 * Change the environment map.
 *
 * @param texture The texture to be used as the environment map.
 */
void EnvironmentLight::SetEnvironmentMap(const std::shared_ptr<Texture>& texture)
{
    // Save the information of the environment map
    m_EnvironmentMap = texture;
    
    // Check for a valid texture
    if (!texture)
        return;
    
    // Update the environment information
    UpdateEnvironment();
    //UpdateSphericalHarmonics();
}

/**
 * Get the irradiance map of the environment light.
 *
 * @return A shared pointer to the irradiance map texture.

const std::shared_ptr<Texture>& EnvironmentLight::GetIrradianceMap()
{
    return m_Framebuffers.Get("Irradiance")->GetColorAttachment(0);
}
 */
/**
 * Get the pre-filter map of the environment light.
 *
 * @return A shared pointer to the pre-filter map texture.

const std::shared_ptr<Texture>& EnvironmentLight::GetPreFilterMap()
{
    return m_Framebuffers.Get("PreFilter")->GetColorAttachment(0);
}
 */
/**
 * @brief Define light properties into the uniforms of the shader program.
 *
 * @param shader The shader program.
 * @param properties The flags indicating which light properties should be defined.
 * @param slot The texture slot to bind the environment map to.
 */
void EnvironmentLight::DefineLightProperties(const std::shared_ptr<Shader> &shader,
                                             LightProperty properties, unsigned int& slot)
{
    // Define the strenght of the ambient light
    shader->SetFloat("u_Environment.La", m_AmbientStrength);
    
    // Define the irradiance information using spherical harmonics
    //auto matrix = flags.IsotropicShading ? m_Coefficients.Isotropic : m_Coefficients.Anisotropic;
    //shader->SetMat4("u_Environment.IrradianceMatrix[0]", matrix.Red);
    //shader->SetMat4("u_Environment.IrradianceMatrix[1]", matrix.Green);
    //shader->SetMat4("u_Environment.IrradianceMatrix[2]", matrix.Blue);
}

void EnvironmentLight::DrawLight()
{
    if (!(m_Model && m_EnvironmentMap))
        return;
    
    RendererCommand::SetDepthFunction(DepthFunction::LEqual);
    
    auto environment = m_Framebuffers.Get("Environment")->GetColorAttachment(0);
    auto material = std::dynamic_pointer_cast<SimpleTextureMaterial>(Renderer::GetMaterialLibrary().Get("CubeMap"));
    material->SetTextureMap(environment);
    
    m_Model->SetMaterial(material);
    m_Model->SetScale(glm::vec3(m_Size));
    
    m_Model->DrawModel();
    
    RendererCommand::SetDepthFunction(DepthFunction::Less);
}

/**
 * Updates the spherical harmonic coefficients for the environment light.

void EnvironmentLight::UpdateSphericalHarmonics()
{
    // Retrieve the spherical harmonics material
    auto material = std::dynamic_pointer_cast<SimpleTextureMaterial>(m_Materials.Get("SphericalHarmonics"));
    if (!material)
        return;
    
    // Create a plane geometry (to render to) using the material
    using VertexData = GeoVertexData<glm::vec4>;
    auto geometry = utils::Geometry::ModelPlane<VertexData>(material);
    geometry->SetScale(glm::vec3(2.0f));
    
    // Get the spherical harmonics framebuffer
    auto& framebuffer = m_Framebuffers.Get("SphericalHarmonics");
    if (!framebuffer)
        return;
    
    // Bind the framebuffer and render the scene to compute the coefficients
    framebuffer->Bind();
    
    Renderer::BeginScene();
    RendererCommand::SetRenderTarget(framebuffer);
    geometry->SetMaterial(material);
    geometry->DrawModel();
    Renderer::EndScene();
    
    framebuffer->Unbind();
    
    // Retrieve the information of the coefficients
    std::vector<char> rawData = framebuffer->GetAttachmentData(0);
    // Ensure the raw data size is compatible with float conversion
    CORE_ASSERT(rawData.size() % sizeof(float) == 0, "Data size is not a multiple of float size!");
    
    // Get the float representation of the data
    const float* dataPointer = reinterpret_cast<const float*>(rawData.data());
    size_t count = rawData.size() / sizeof(float);
    std::vector<float> data(dataPointer, dataPointer + count);
    
    // Compute the coefficients
    m_Coefficients.UpdateIsotropicMatrix(data);
    m_Coefficients.UpdateAnisotropicMatrix(data);
}
 */
/**
 * Updates the environment lighting information.
 *
 * This function renders various maps needed for environment-based lighting:
 * - Cube map representing the environment.
 * - Irradiance map for diffuse lighting.
 * - Pre-filtered environment map for specular lighting.
 *
 * The function uses a pre-defined set of view matrices and a projection matrix
 * for rendering the maps. It updates the current texture representing the environment
 * map and the size of the environment (cube) model.
 *
 * @param views View matrices for each of the six cube map faces.
 * @param projection Projection matrix for rendering the cube maps.
 */
void EnvironmentLight::UpdateEnvironment()
{
    // Define a cubemap to be rendered
    auto cubemap = utils::cubemap::BuildCubeMap(RendererAPI::GetAPI());
    // Apply rotation to the view matrices of the cubemap
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(m_Rotation)));
    utils::cubemap::RotateCubeMap(cubemap, rotation);
    
    // Get the material library defined in the renderer
    auto& materialLibrary = Renderer::GetMaterialLibrary();
    // Update the current texture representing the environment map
    auto equirectangularMaterial = std::dynamic_pointer_cast<SimpleTextureMaterial>(materialLibrary.Get("EquirectangularMap"));
    if (equirectangularMaterial)
        equirectangularMaterial->SetTextureMap(m_EnvironmentMap);
    
    // Render the environment map into a cube map configuration
    utils::cubemap::RenderCubeMap(cubemap, m_Model, equirectangularMaterial, m_Framebuffers.Get("Environment"));
    
    // TODO: remove this and set it into another function. Make the static variables an enumeration.
    /*
     // Render the irradiance map
     RenderCubeMap(viewMatrix, projectionMatrix, m_Materials.Get("Irradiance"),
     m_Framebuffers.Get("Irradiance"));
     
     // Render the pre-filter map
     static const unsigned int maxMipMapLevel = 5;
     for (unsigned int mip = 0; mip < maxMipMapLevel; ++mip)
     {
     // Define the roughness
     float roughness = (float)mip / (float)(maxMipMapLevel - 1);
     m_Materials.Get("PreFilter")->GetShader()->SetFloat("u_Material.Roughness", roughness);
     
     // reisze framebuffer according to mip-level size.
     unsigned int mipWidth  = 128 * std::pow(0.5, mip);
     unsigned int mipHeight = 128 * std::pow(0.5, mip);
     
     // Render into the cubemap
     RenderCubeMap(viewMatrix, projectionMatrix, m_Materials.Get("PreFilter"),
     m_Framebuffers.Get("PreFilter"),
     mipWidth, mipHeight, mip, false);
     }
     */
}

/**
 * Updates the isotropic SH matrices for all color channels.
 *
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).

void SHCoefficients::UpdateIsotropicMatrix(const std::vector<float> &shCoeffs)
{
    Isotropic.Red = GenerateIsotropicMatrix(0, shCoeffs);
    Isotropic.Green = GenerateIsotropicMatrix(1, shCoeffs);
    Isotropic.Blue = GenerateIsotropicMatrix(2, shCoeffs);
}
 */
/**
 * Updates the anisotropic SH matrices for all color channels.
 *
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).

void SHCoefficients::UpdateAnisotropicMatrix(const std::vector<float> &shCoeffs)
{
    Anisotropic.Red = GenerateAnisotropicMatrix(0, shCoeffs);
    Anisotropic.Green = GenerateAnisotropicMatrix(1, shCoeffs);
    Anisotropic.Blue = GenerateAnisotropicMatrix(2, shCoeffs);
}
 */
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

glm::mat4 SHCoefficients::GenerateIsotropicMatrix(int colorIndex,
                                                  const std::vector<float> &shCoeffs)
{
    float c1 = 0.429043f;
    float c2 = 0.511664f;
    float c3 = 0.743125f;
    float c4 = 0.886227f;
    float c5 = 0.247708f;
    
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
    - c5 * shCoeffs[6 * 3 + colorIndex];    // L00 and L20
    
    return M;
}
 */
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

glm::mat4 SHCoefficients::GenerateAnisotropicMatrix(int colorIndex,
                                                    const std::vector<float> &shCoeffs)
{
    float b0 =  0.282095f;
    float b1 = -0.0682843f;
    float b2 = -0.118272f;
    float b3 =  0.0394239f;
    
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
 */
} // namespace pixc
