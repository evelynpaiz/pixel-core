#include "Examples/BasicPrimitive.h"

/**
 * @brief Render the rendering layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void BasicPrimitive::OnUpdate(pixc::Timestep ts)
{
    // Reset per-frame rendering statistics (draw calls, triangles, etc.)
    pixc::Renderer::ResetStats();
    
    // Begin a new render pass
    pixc::RendererCommand::BeginRenderPass();
    // Set viewport to cover the entire layer
    pixc::RendererCommand::SetViewport(0, 0, m_Width, m_Height);
    
    // Set clear color for the framebuffer (background color)
    pixc::RendererCommand::SetClearColor(glm::vec4(0.2f, 0.25f, 0.3f, 1.0f));
    // Clear the framebuffer
    pixc::RendererCommand::Clear();
    
    // Begin scene rendering
    pixc::Renderer::BeginScene();
    // Set material color for the primitive
    m_Material->SetColor(glm::vec4(0.5f, 1.0f, 0.7f, 1.0f));
    // Assign material to the model
    m_Model->SetMaterial(m_Material);
    // Draw the model
    m_Model->DrawModel();
    
    // End scene rendering
    pixc::Renderer::EndScene();
    // End the render pass
    pixc::RendererCommand::EndRenderPass();
}

/**
 * @brief Called when the rendering layer is attached to the application.
 */
void BasicPrimitive::OnAttach()
{
    // Create a simple color material for the primitive
    m_Material = std::make_shared<pixc::UnlitMaterial>();
    
    // Define the vertex structure used for this mesh
    struct SimpleVertex
    {
        glm::vec4 position;         ///< Vertex position in homogeneous coordinates (x, y, z, w)
    };
    // Define vertices of the triangle (3 corners)
    std::vector<SimpleVertex> vertices = {
        { glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f) },   // left
        { glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f) },   // right
        { glm::vec4( 0.0f,  0.5f, 0.0f, 1.0f) }    // top
    };
    // Define the indices that form the triangle from the vertices
    std::vector<uint32_t> indices = {  0, 1, 2 };
    
    // Define the layout of the vertex buffer,
    // each entry describes the attribute name and its data type
    pixc::BufferLayout layout =
    {
        { "a_Position", { pixc::DataType::Vec4 } }
    };
    
    // Create the mesh and define its vertices, indices, and layout
    pixc::Mesh<SimpleVertex> mesh;
    mesh.DefineMesh(vertices, indices, layout);
    // Create a model from the mesh and store it for rendering
    m_Model = std::make_shared<pixc::Model<SimpleVertex>>(mesh);
}

/**
 * @brief Handle an event that possibly occurred inside the viewer layer.
 * @param e Event.
 */
void BasicPrimitive::OnEvent(pixc::Event& e)
{
    // Define the event dispatcher
    pixc::EventDispatcher dispatcher(e);
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<pixc::WindowResizeEvent>(BIND_EVENT_FN(BasicPrimitive::OnWindowResize));
}

/**
 * @brief Function to be called when a window resize event happens.
 *
 * @param e Event to be handled.
 * @return `True` if the event has been handled.
 */
bool BasicPrimitive::OnWindowResize(pixc::WindowResizeEvent &e)
{
    // Notify of the change
    PIXEL_CORE_TRACE("Window resized to {0} x {1}", e.GetWidth(), e.GetHeight());
    
    // Update the viewport size
    m_Width = e.GetWidth();
    m_Height = e.GetHeight();
    
    // Define the event as handled
    return true;
}
