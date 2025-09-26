#include "pixcpch.h"
#include "Foundation/Layer/LayerStack.h"

namespace pixc {

/**
 * Empty resources and delete the stack.
 */
LayerStack::~LayerStack()
{
    // Detach all layers in the stack
    for (std::shared_ptr<Layer> layer : m_Layers)
        layer->OnDetach();
}

/**
 * Add a rendering layer to the stack.
 *
 * @param layer New rendering layer.
 */
void LayerStack::PushLayer(const std::shared_ptr<Layer>& layer)
{
    // Add the rendering layer above all layers in the stack
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
    
    layer->OnAttach();
}

/**
 * Add an overlay layer (rendered on top) to the stack.
 *
 * @param overlay New overlay layer.
 */
void LayerStack::PushOverlay(const std::shared_ptr<Layer>& overlay)
{
    // Add the overlay layer on top of all layers in the stack
    m_Layers.emplace_back(overlay);
    overlay->OnAttach();
}

/**
 * Remove a rendering layer from the stack.
 *
 * @param layer Rendering layer.
 */
void LayerStack::PopLayer(const std::shared_ptr<Layer>& layer)
{
    // Check if the stack contains the rendering layer
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    
    // If the layer exists, remove it
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

/**
 * Remove an overlay (rendered on top) layer from the stack.
 *
 * @param overlay Overlay layer.
 */
void LayerStack::PopOverlay(const std::shared_ptr<Layer>& overlay)
{
    // Check if the stack contains the overlay layer
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    
    // If the layer exists, remove it
    if (it != m_Layers.end())
    {
        overlay->OnDetach();
        m_Layers.erase(it);
    }
}

} // namespace pixc
