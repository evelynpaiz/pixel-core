#pragma once

#include "Common/Renderer/RendererCommand.h"

#include "Common/Renderer/Drawable/Model/Model.h"
#include "Common/Renderer/Drawable/Model/ModelUtils.h"

#include "Common/Renderer/Material/SimpleMaterial.h"

/**
 * Represents the viewport settings and geometry for rendering.
 *
 * The `Viewport` class encapsulates the properties of the rendering viewport,
 * including its width, height, 3D geometry model, framebuffer for rendering,
 * and the material used to display the rendered image.
 */
class Viewport
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Construct a viewport with a specific size.
    /// @param width The width of the viewport.
    /// @param height The height of the viewport.
    Viewport(int width, int height, const std::filesystem::path& shaderPath = "") :
        m_Width(width), m_Height(height)
    {
        // Define the framebuffer to be render into and update its information
        FrameBufferSpecification viewportSpec;
        viewportSpec.SetFrameBufferSize(width, height);
        viewportSpec.AttachmentsSpec = {
            { TextureType::TEXTURE2D, TextureFormat::RGBA8 },
            { TextureType::TEXTURE2D, TextureFormat::DEPTH16 }
        };
        m_Framebuffer = FrameBuffer::Create(viewportSpec);
        
        // Define the material to be displayed in the viewport
        m_Material = shaderPath.empty() ? std::make_shared<SimpleTextureMaterial>() :
                                          std::make_shared<SimpleTextureMaterial>(shaderPath);
        m_Material->SetTextureMap(m_Framebuffer->GetColorAttachment(0));
        
        // Create the geometric model of the viewport
        using VertexData = GeoVertexData<glm::vec4, glm::vec2>;
        m_Geometry = utils::Geometry::ModelPlane<VertexData>();
        m_Geometry->SetScale(glm::vec3(2.0f));
    }
    /// @brief Delete the viewport.
    ~Viewport() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the width of the viewport.
    /// @return Size (width).
    int GetWidth() const { return m_Width; }
    /// @brief Get the height of the viewport.
    /// @return Size (height).
    int GetHeight() const { return m_Height; }
    
    /// @brief Get the framebuffer with the rendered output.
    /// @return Viewport framebuffer.
    const std::shared_ptr<FrameBuffer>& GetFramebuffer() const { return m_Framebuffer; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Set the width of the viewport.
    /// @param width The size (width).
    void SetWidth(int width) { m_Width = width; }
    /// @brief Set the height of the viewport.
    /// @param height The size (height).
    void SetHeight(int height) { m_Height = height; }
    
    /// @brief Resize the size of the viewport.
    /// @param width The width of the viewport.
    /// @param height The height of the viewport.
    void Resize(int width, int height)
    {
        m_Width = width;
        m_Height = height;
        m_Framebuffer->Resize(width, height);
    }
    
    // Render
    // ----------------------------------------
    /// @brief Display the rendered image into the viewport.
    /// @param material Viewport material.
    void Render()
    {
        RendererCommand::SetViewport(0, 0, m_Width, m_Height);
        RendererCommand::SetRenderTarget();
        
        Renderer::BeginScene();
        m_Geometry->SetMaterial(m_Material);
        m_Geometry->DrawModel();
        Renderer::EndScene();
    }
    /// @brief Render the viewport geometry into a framebuffer.
    /// @param framebuffer The output of the rendered image.
    /// @param material The shading material used for rendering.
    void RenderToFramebuffer(const std::shared_ptr<FrameBuffer>& framebuffer,
                             const std::shared_ptr<Material>& material)
    {
        framebuffer->Bind();
        
        RendererCommand::SetRenderTarget(framebuffer);
        
        Renderer::BeginScene();
        m_Geometry->SetMaterial(material);
        m_Geometry->DrawModel();
        Renderer::EndScene();
        
        framebuffer->Unbind();
    }
    
    // Viewport variables
    // ----------------------------------------
private:
    ///< Width of the viewport.
    int m_Width;
    ///< Height of the viewport.
    int m_Height;
    ///< 3D geometry of the viewport.
    std::shared_ptr<BaseModel> m_Geometry;
    
    ///< Framebuffer to render into.
    std::shared_ptr<FrameBuffer> m_Framebuffer;
    ///< Material to be used to display the framebuffer.
    std::shared_ptr<SimpleTextureMaterial> m_Material;
    
    // Friend classes
    // ----------------------------------------
public:
    friend class Scene;
};
