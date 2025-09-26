#pragma once

#include "Foundation/Core/ClassUtils.h"

#include "Foundation/Renderer/Drawable/Model/Model.h"
#include "Foundation/Renderer/Drawable/Model/ModelUtils.h"

#include "Foundation/Renderer/Material/SimpleMaterial.h"

#include "Foundation/Renderer/RendererCommand.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a viewport used for rendering.
 *
 * The `Viewport` class encapsulates a renderable area with its own framebuffer, material,
 * and a geometric plane for displaying the rendered image.
 * It supports both supersampling and subsampling through a scale factor, allowing the framebuffer
 * to be rendered at higher or lower resolutions than the display size while keeping the geometry consistent.
 *
 * Copying or moving `Viewport` objects is disabled to ensure proper management of the
 * underlying framebuffer and shared geometry.
 */
class Viewport
{
public:
    // Constructor(s)/ Destructor
    // ----------------------------------------
    /// @brief Construct a viewport with a specific size.
    /// @param width The width of the viewport.
    /// @param height The height of the viewport.
    /// @param shaderPath Optional shader path for the viewport material.
    Viewport(uint32_t width, uint32_t height, const std::filesystem::path& shaderPath = "")
    {
        // Create the output framebuffer (screen buffer)
        FrameBufferSpecification viewportSpec;
        viewportSpec.SetFrameBufferSize(width, height);
        viewportSpec.AttachmentsSpec = {
            { TextureType::TEXTURE2D, TextureFormat::RGBA8 },
            { TextureType::TEXTURE2D, TextureFormat::DEPTH16 }
        };
        m_ScreenBuffer = FrameBuffer::Create(viewportSpec);
        
        // Initialize shared geometry if it hasn't been created yet
        if (!s_Geometry)
        {
            using VertexData = GeoVertexData<glm::vec4, glm::vec2>;
            s_Geometry = utils::geometry::ModelPlane<VertexData>();
            s_Geometry->SetScale(glm::vec3(2.0f));
        }
        
        // Determine material name
        std::string name = shaderPath.empty() ? "Viewport" : "Viewport" + shaderPath.stem().string();
        // Lambda to create or get material from the library
        auto createOrGetMaterial = [&](const std::string& name, const auto& path)
        {
            auto& materialLibrary = Renderer::GetMaterialLibrary();
            if (!materialLibrary.Exists(name)) {
                return path.empty() ?
                    materialLibrary.Create<SimpleTextureMaterial>(name) :
                    materialLibrary.Create<SimpleTextureMaterial>(name, path);
            }
            return std::dynamic_pointer_cast<SimpleTextureMaterial>(materialLibrary.Get(name));
        };
        m_Material = createOrGetMaterial(name, shaderPath);
    }
    /// @brief Delete the viewport.
    ~Viewport() = default;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the width of the viewport.
    /// @return Size (width).
    uint32_t GetWidth() const { return m_ScreenBuffer->GetSpec().Width / m_Scale; }
    /// @brief Get the height of the viewport.
    /// @return Size (height).
    uint32_t GetHeight() const { return m_ScreenBuffer->GetSpec().Height / m_Scale; }
    
    /// @brief Get the framebuffer with the rendered output.
    /// @return Viewport framebuffer.
    const std::shared_ptr<FrameBuffer>& GetScreenBuffer() const { return m_ScreenBuffer; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Resize the size of the viewport.
    /// @param width The width of the viewport.
    /// @param height The height of the viewport.
    void Resize(uint32_t width, uint32_t height)
    {
        m_ScreenBuffer->Resize(width * m_Scale, height * m_Scale);
    }
    /// @brief Set a scale factor for subsampling the screen buffer.
    /// @param scale Scale factor (e.g., 0.5 = half-resolution rendering)
    void ReScale(float scale)
    {
        uint32_t width = GetWidth();
        uint32_t height = GetHeight();
        
        m_Scale = scale;
        Resize(width, height);
    }
    
    // Render
    // ----------------------------------------
    /// @brief Display the rendered image into the screen.
    /// @param material Viewport material.
    void RenderToScreen() const
    {
        m_Material->SetTextureMap(m_ScreenBuffer->GetColorAttachment(0));
        Render(nullptr, m_Material);
    }
    /// @brief Render the viewport geometry into a framebuffer.
    /// @param framebuffer The output of the rendered image.
    /// @param material The shading material used for rendering.
    void RenderToFrameBuffer(const std::shared_ptr<FrameBuffer>& framebuffer,
                             const std::shared_ptr<Material>& material) const
    {
        Render(framebuffer, material);
    }
    
    // Friend class definition(s)
    // ----------------------------------------
    friend class Scene;
    
private:
    /// @brief Render the viewport geometry into a framebuffer.
    /// @param framebuffer The output of the rendered image.
    /// @param material The shading material used for rendering.
    void Render(const std::shared_ptr<FrameBuffer>& framebuffer,
                const std::shared_ptr<Material>& material) const
    {
        RendererCommand::BeginRenderPass(framebuffer);
        if (!framebuffer)
            RendererCommand::SetViewport(0, 0, GetWidth(), GetHeight());
        RendererCommand::SetClearColor(glm::vec4(0.0f));
        RendererCommand::Clear();

        Renderer::BeginScene();
        s_Geometry->SetMaterial(material);
        s_Geometry->DrawModel();
        Renderer::EndScene();

        RendererCommand::EndRenderPass();
    }
    
    // Viewport variables
    // ----------------------------------------
private:
    ///< 3D geometry of the viewport.
    static std::shared_ptr<BaseModel> s_Geometry;
    
    ///< Framebuffer to render into.
    std::shared_ptr<FrameBuffer> m_ScreenBuffer;
    ///< Material to be used to display the framebuffer.
    std::shared_ptr<SimpleTextureMaterial> m_Material;
    
    ///< Scale factor for the viewport plane (for subsampling or resizing)
    float m_Scale = 1.0f;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Viewport);
};

// Define the static shared geometry
inline std::shared_ptr<BaseModel> Viewport::s_Geometry = nullptr;

} // namespace pixc
