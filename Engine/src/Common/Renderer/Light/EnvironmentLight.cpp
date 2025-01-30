#include "enginepch.h"
#include "Common/Renderer/Light/EnvironmentLight.h"

#include "Common/Renderer/RendererCommand.h"

#include "Common/Renderer/Texture/Texture.h"
#include "Common/Renderer/Texture/TextureCube.h"

#include "Common/Renderer/Light/PositionalLight.h"

/**
 * Generate an environment light source in the world space.
 *
 * @param width The width that the light source covers.
 * @param height The height that the light source covers.
 * @param color The color of the light source.
 * @param position The position of the light source.
 */
EnvironmentLight::EnvironmentLight(const unsigned int width, const unsigned int height)
{
    // Define the framebuffer(s) for the environment
    const static unsigned int scale = 4;
    InitEnvironmentFramebuffers(scale * width);

    // Define the material(s) for the environment
    InitEnvironmentMaterials();

    // Define 3D model of the light source
    using VertexData = GeoVertexData<glm::vec4>;
    m_Model = utils::Geometry::ModelCube<VertexData>(m_Materials.Get("Environment"));
}

/**
 * Initialize framebuffers for the environment light.
 *
 * @param cubeSize Cube map resolution (width = height).
 */
void EnvironmentLight::InitEnvironmentFramebuffers(const unsigned int cubeSize)
{
    FrameBufferSpecification spec;
    
    spec.SetFrameBufferSize(3, 3);
    TextureSpecification sh(TextureType::TEXTURE2D, TextureFormat::RGB16F);
    sh.Filter = TextureFilter::Nearest;
    spec.AttachmentsSpec = { { sh } };
    m_Framebuffers.Create("SphericalHarmonics", spec);
    
    // -------
    
    spec.SetFrameBufferSize(cubeSize, cubeSize);
    spec.AttachmentsSpec = {
        { TextureType::TEXTURE2D, TextureFormat::DEPTH24 },
        { TextureType::TEXTURECUBE, TextureFormat::RGB16F }
    };
    
    spec.MipMaps = true;
    m_Framebuffers.Create("Environment", spec);
    
    spec.SetFrameBufferSize(32, 32);
    spec.MipMaps = false;
    m_Framebuffers.Create("Irradiance", spec);
    
    spec.SetFrameBufferSize(128, 128);
    spec.MipMaps = true;
    m_Framebuffers.Create("PreFilter", spec);
}

/**
 * Initialize materials for the environment light.
 */
void EnvironmentLight::InitEnvironmentMaterials()
{
    auto environment = m_Framebuffers.Get("Environment")->GetColorAttachment(0);
    
    // Equirectangular mapping
    m_Materials.Create<SimpleTextureMaterial>("Equirectangular",
        "Resources/shaders/environment/EquirectangularMap.glsl");
    
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
    UpdateSphericalHarmonics();
}

/**
 * Get the irradiance map of the environment light.
 *
 * @return A shared pointer to the irradiance map texture.
 */
const std::shared_ptr<Texture>& EnvironmentLight::GetIrradianceMap()
{
    return m_Framebuffers.Get("Irradiance")->GetColorAttachment(0);
}

/**
 * Get the pre-filter map of the environment light.
 *
 * @return A shared pointer to the pre-filter map texture.
 */
const std::shared_ptr<Texture>& EnvironmentLight::GetPreFilterMap()
{
    return m_Framebuffers.Get("PreFilter")->GetColorAttachment(0);
}

/**
 * @brief Define light properties into the uniforms of the shader program.
 *
 * @param shader The shader program.
 * @param flags The flags indicating which light properties should be defined.
 * @param slot The texture slot to bind the environment map to.
 */
void EnvironmentLight::DefineLightProperties(const std::shared_ptr<Shader> &shader,
                                             const LightFlags &flags,
                                             unsigned int& slot)
{
    // Define the strenght of the ambient light
    shader->SetFloat("u_Environment.La", m_AmbientStrength);
    
    // Define the irradiance information using spherical harmonics
    auto matrix = flags.IsotropicShading ? m_Coefficients.Isotropic : m_Coefficients.Anisotropic;
    shader->SetMat4("u_Environment.IrradianceMatrix[0]", matrix.Red);
    shader->SetMat4("u_Environment.IrradianceMatrix[1]", matrix.Green);
    shader->SetMat4("u_Environment.IrradianceMatrix[2]", matrix.Blue);
}

/**
 * Updates the spherical harmonic coefficients for the environment light.
 */
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
    // Define the views to render each face of the cube
    static const glm::mat4 POSITIVE_X_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    static const glm::mat4 NEGATIVE_X_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    
    static const glm::mat4 POSITIVE_Y_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
    static const glm::mat4 NEGATIVE_Y_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
    
    static const glm::mat4 POSITIVE_Z_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    static const glm::mat4 NEGATIVE_Z_VIEW = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
    
    // Define the tranformation matrices
    const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    const std::array<glm::mat4, 6>& viewMatrix =
    {
        POSITIVE_X_VIEW, NEGATIVE_X_VIEW, POSITIVE_Y_VIEW, NEGATIVE_Y_VIEW, POSITIVE_Z_VIEW, NEGATIVE_Z_VIEW
    };
    
    // Apply rotation to each view matrix
    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(m_Rotation)));
    std::array<glm::mat4, 6> sceneViewMatrix;
    for (size_t i = 0; i < viewMatrix.size(); ++i)
        sceneViewMatrix[i] = viewMatrix[i] * rotation;

    // Update the current texture representing the environment map
    auto equirectangularMaterial = std::dynamic_pointer_cast<SimpleTextureMaterial>(m_Materials.Get("Equirectangular"));
    if (equirectangularMaterial)
        equirectangularMaterial->SetTextureMap(m_EnvironmentMap);
    
    // Update the size of the environment (cube) model
    m_Model->SetScale(glm::vec3(2.0f));
    
    // Render the environment map into a cube map configuration
    RenderCubeMap(sceneViewMatrix, projectionMatrix, m_Materials.Get("Equirectangular"),
                  m_Framebuffers.Get("Environment"));
    
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
    
    m_Framebuffers.Get("PreFilter")->Unbind(false);
    m_Model->SetMaterial(m_Materials.Get("Environment"));
    m_Model->SetScale(glm::vec3(70.0f));
}

/**
 * Render a cube map from multiple perspectives using the specified material and framebuffer.
 *
 * @param views An array of view matrices representing the six cube map faces.
 * @param projection The projection matrix for the rendering.
 * @param material The material to be used for rendering each cube map face.
 * @param framebuffer The framebuffer where the cube map is rendered.
 * @param level The mip level of the framebuffer to which the cube map is rendered.
 * @param genMipMaps A flag indicating whether to generate mipmaps for the resulting cube map.
 */
void EnvironmentLight::RenderCubeMap(const std::array<glm::mat4, 6>& views, const glm::mat4 &projection,
                                     const std::shared_ptr<Material>& material,
                                     const std::shared_ptr<FrameBuffer>& framebuffer,
                                     const unsigned int& viewportWidth, const unsigned int& viewportHeight,
                                     const unsigned int &level, const bool &genMipMaps)
{
    // Set the material for rendering
    m_Model->SetMaterial(material);

    // Loop through each face of the cube map
    for (unsigned int i = 0; i < views.size(); ++i)
    {
        // Bind the framebuffer for drawing to the current cube map face and mip level
        framebuffer->BindForDrawAttachmentCube(0, i, level);

        // Set the viewport dimensions if provided
        if (viewportWidth > 0 && viewportHeight > 0)
            RendererCommand::SetViewport(0, 0, viewportWidth, viewportHeight);

        // Clear the active buffers in the framebuffer
        RendererCommand::SetRenderTarget(framebuffer);
        
        // Begin rendering scene with the specified view and projection matrices
        Renderer::BeginScene(views[i], projection);
        // Draw the model using the specified material
        m_Model->DrawModel();
        // End the rendering scene
        Renderer::EndScene();
    }

    // Unbind the framebuffer and optionally generate mipmaps
    framebuffer->Unbind(genMipMaps);
}

/**
 * Updates the isotropic SH matrices for all color channels.
 * 
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
 */
void SHCoefficients::UpdateIsotropicMatrix(const std::vector<float> &shCoeffs)
{
    Isotropic.Red = GenerateIsotropicMatrix(0, shCoeffs);
    Isotropic.Green = GenerateIsotropicMatrix(1, shCoeffs);
    Isotropic.Blue = GenerateIsotropicMatrix(2, shCoeffs);
}

/**
 * Updates the anisotropic SH matrices for all color channels.
 *
 * @param shCoeffs A vector containing the 9 spherical harmonic coefficients (L00, L1-1, L10, ..., L22),
 *                 stored as floats with interleaved RGB values (e.g., R00, G00, B00, R1-1, G1-1, B1-1, ...).
 */
void SHCoefficients::UpdateAnisotropicMatrix(const std::vector<float> &shCoeffs)
{
    Anisotropic.Red = GenerateAnisotropicMatrix(0, shCoeffs);
    Anisotropic.Green = GenerateAnisotropicMatrix(1, shCoeffs);
    Anisotropic.Blue = GenerateAnisotropicMatrix(2, shCoeffs);
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
