#include "pixcpch.h"
#include "Foundation/Renderer/Light/Environment/EnvironmentLight.h"

#include "Foundation/Renderer/Material/UnlitMaterial.h"

#include "Foundation/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"
#include "Foundation/Renderer/Utils/CubeMapUtils.h"

namespace pixc {

/**
 * @brief Construct an environment ligth in the scene.
 *
 * @param size The resolution of the cubemap (width = height).
 */
EnvironmentLight::EnvironmentLight(const uint32_t size) : Light()
{
    // Initialize the environment light
    InitEnvironment(size);
}

/**
 * @brief Setup the framebuffers for the environment light.
 *
 * @param size The resolution of the cubemap (width = height).
 */
void EnvironmentLight::InitEnvironment(const uint32_t size)
{
    SetupFrameBuffers(size);
    SetupResources();
}

/**
 * Initialize framebuffers for the environment light.
 *
 * @param size The resolution of the cubemap (width = height).
 */
void EnvironmentLight::SetupFrameBuffers(const uint32_t size)
{
    // Create a framebuffer specification for the environment cubemap
    FrameBufferSpecification spec;

    // Set the framebuffer size (width = height = size)
    spec.SetFrameBufferSize(size, size);

    // Define the attachments:
    // - A cubemap texture for storing the environment map (RGB16F for high dynamic range)
    // - A 2D depth texture to use as the depth buffer (DEPTH24)
    spec.AttachmentsSpec = {
        { TextureType::TEXTURECUBE, TextureFormat::RGB16F },
        { TextureType::TEXTURE2D, TextureFormat::DEPTH24 }
    };

    // Enable mipmap generation for the environment cubemap
    spec.MipMaps = true;

    // Create the framebuffer and store it in the library
    m_FrameBuffers.Create("Environment", spec);
    
    // -------
    
    //spec.SetFrameBufferSize(32, 32);
    //spec.MipMaps = false;
    //m_Framebuffers.Create("Irradiance", spec);
    
    //spec.SetFrameBufferSize(128, 128);
    //spec.MipMaps = true;
    //m_Framebuffers.Create("PreFilter", spec);
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
        materialLibrary.Create<TextureMaterial>("EquirectangularMap",
                                                "pixc/shaders/environment/EquirectangularMap");
    }
    // Cube mapping
    if (!materialLibrary.Exists("CubeMap"))
    {
        materialLibrary.Create<TextureMaterial>("CubeMap",
                                                "pixc/shaders/environment/CubeMap");
    }
    
    // Define 3D model of the light source
    using VertexData = GeoVertexData<glm::vec4>;
    m_Model = utils::geometry::ModelCube<VertexData>(materialLibrary.Get("EquirectangularMap"));
    m_Model->SetScale(glm::vec3(2.0f));
    
    /*
    // Irradiance mapping
    auto irradiance = m_Materials.Create<TextureMaterial>("Irradiance",
                                                          "Resources/shaders/environment/IrradianceMap.glsl");
    irradiance->SetTextureMap(environment);
    
    // Pre-filtering mapping
    auto preFilter = m_Materials.Create<TextureMaterial>("PreFilter",
                                                         "Resources/shaders/environment/PreFilterMap.glsl");
    preFilter->SetTextureMap(environment);
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
                                             LightProperty properties)
{
    // Define the strenght of the ambient light
    shader->SetFloat("u_Environment.La", m_AmbientStrength);
}

/**
 * @brief Renders the 3D model that represents the environment.
 */
void EnvironmentLight::DrawLight()
{
    if (!(m_Model && m_EnvironmentMap))
        return;
    
    RendererCommand::SetDepthFunction(DepthFunction::LEqual);
    
    auto environment = m_FrameBuffers.Get("Environment")->GetColorAttachment(0);
    auto material = std::dynamic_pointer_cast<TextureMaterial>(Renderer::GetMaterialLibrary().Get("CubeMap"));
    material->SetTextureMap(environment);
    
    m_Model->SetMaterial(material);
    m_Model->DrawModel();
    
    RendererCommand::SetDepthFunction(DepthFunction::Less);
}

/**
 * Updates the environment lighting information.
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
    auto material = std::dynamic_pointer_cast<TextureMaterial>(materialLibrary.Get("EquirectangularMap"));
    material->SetTextureMap(m_EnvironmentMap);
    
    // Render the environment map into a cube map configuration
    utils::cubemap::RenderCubeMap(cubemap, m_Model, material, m_FrameBuffers.Get("Environment"));
    
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

} // namespace pixc
