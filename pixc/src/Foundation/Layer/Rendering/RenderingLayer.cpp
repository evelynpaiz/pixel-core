#include "Foundation/Layer/Rendering/RenderingLayer.h"

#include "Foundation/Event/WindowEvent.h"

#include "Foundation/Renderer/Renderer.h"
#include "Foundation/Renderer/RendererCommand.h"

#include "Foundation/Renderer/Camera/PerspectiveCamera.h"

#include "Foundation/Renderer/Texture/Texture2D.h"

//#include "Foundation/Renderer/Material/Material.h"
#include "Foundation/Renderer/Shader/Shader.h"

//#include <glm/gtc/matrix_transform.hpp>

namespace pixc {

/**
 * @brief Define a layer for a 3D viewer.
 */
RenderingLayer::RenderingLayer(int width, int height,
                               const std::string& name)
: Layer(name)
{
    // Define the rendering camera
    m_Camera = std::make_shared<PerspectiveCamera>(width, height);
    m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.5f));
}

/**
 * @brief Attach (add) the viewer layer to the rendering engine.
 */
void RenderingLayer::OnAttach()
{
    // Define the shader to be used
    m_Shader = Shader::Create("Simple", "Resources/shaders/base/SimpleColorTexture.metal");
    
    // Create the drawable that will be rendered
    m_Drawable = Drawable::Create();
    
    // Define the index data for the drawable
    std::vector<unsigned int> indices =
    {
        0, 1, 2,    // first triangle
        2, 3, 0,    // second triangle
    };
    m_Drawable->SetIndexData(indices);
    
    // Define the vertex data for the drawable
    std::vector<RenderingLayer::VertexData> vertices =
    {
        // { position, texture coords }
        { {-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f,  0.0f} },     // bottom left (0)
        { { 0.5f, -0.5f, 0.0f, 1.0f}, {1.0f,  0.0f} },     // bottom right (1)
        { { 0.5f,  0.5f, 0.0f, 1.0f}, {1.0f,  1.0f} },     // top right (2)
        { {-0.5f,  0.5f, 0.0f, 1.0f}, {0.0f,  1.0f} }      // top left (3)
    };
    
    BufferLayout layout = {
        { "a_Position", DataType::Vec4 },
        { "a_TextureCoord", DataType::Vec2 },
    };
    
    m_Drawable->AddVertexData(vertices, layout);
    m_Drawable->SetShader(m_Shader);
}

/**
 * @brief Render the viewer layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void RenderingLayer::OnUpdate(Timestep ts)
{
    // Define rendering texture(s)
    // Define a basic filtering
    TextureSpecification spec;
    
    static auto &white = utils::textures::WhiteTexture2D();
    static auto container = Texture2D::CreateFromFile("resources/textures/container.jpg", spec);
    
    // Reset rendering statistics
    Renderer::ResetStats();
    
    // Render
    RendererCommand::BeginRenderPass();
    RendererCommand::SetClearColor(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f));
    RendererCommand::Clear();
    
    Renderer::BeginScene(m_Camera);
    
    m_Shader->Bind();
    m_Shader->SetVec4("u_Material.Color", glm::vec4(1.0f));
    m_Shader->SetTexture("u_Material.TextureMap", container, 0);
    
    m_Shader->SetMat4("u_Transform.Model", glm::mat4(1.0f));
    m_Shader->SetMat4("u_Transform.View", m_Camera->GetViewMatrix());
    m_Shader->SetMat4("u_Transform.Projection", m_Camera->GetProjectionMatrix());
    m_Shader->Unbind();
    
    Renderer::Draw(m_Drawable);
    Renderer::EndScene();
    
    RendererCommand::EndRenderPass();
    
    // Update camera
    m_Camera->OnUpdate(ts);
}

/**
 * @brief Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void RenderingLayer::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<WindowResizeEvent>(
                                           BIND_EVENT_FN(RenderingLayer::OnWindowResize));
    
    // Handle the events on the camera
    m_Camera->OnEvent(e);
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return True if the event has been handled.
 */
bool RenderingLayer::OnWindowResize(WindowResizeEvent &e)
{
    // Update the camera
    m_Camera->SetViewportSize(e.GetWidth(), e.GetHeight());
    
    //m_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
    
    PIXEL_CORE_TRACE("Window resized to {0} x {1}", e.GetWidth(), e.GetHeight());
    return true;
}

} // namespace pixc
