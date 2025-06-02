#pragma once

#include "Common/Layer/Layer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Manages a stack of layers in the rendering engine.
 *
 * The `LayerStack` class provides a way to manage a stack of layers in the rendering engine.
 * Layers can be pushed and popped onto the stack, allowing for dynamic management of the rendering
 * order. It also provides iterator methods for iterating over the layers in the stack.
 */
class LayerStack
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Generate an empty stack of layers.
    LayerStack() = default;
    ~LayerStack();
    
    // Stack management
    // ----------------------------------------
    void PushLayer(const std::shared_ptr<Layer>& layer);
    void PushOverlay(const std::shared_ptr<Layer>& overlay);
    void PopLayer(const std::shared_ptr<Layer>& layer);
    void PopOverlay(const std::shared_ptr<Layer>& overlay);
    
    // Iterator
    // ----------------------------------------
    /// @brief Get an iterator pointing to the bottom rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::iterator begin()
    {
        return m_Layers.begin();
    }
    /// @brief Get an iterator pointing to the top rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::iterator end()
    {
        return m_Layers.end();
    }
    /// @brief Get an iterator pointing to the top rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rbegin()
    {
        return m_Layers.rbegin();
    }
    /// @brief Get an iterator pointing to the bottom rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::reverse_iterator rend()
    {
        return m_Layers.rend();
    }
    
    /// @brief Get an iterator pointing to the bottom rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::const_iterator begin() const
    {
        return m_Layers.begin();
    }
    /// @brief Get an iterator pointing to the top rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::const_iterator end() const
    {
        return m_Layers.end();
    }
    /// @brief Get an iterator pointing to the top rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rbegin() const
    {
        return m_Layers.rbegin();
    }
    /// @brief Get an iterator pointing to the bottom rendered layer.
    /// @return An iterator of the layer stack.
    std::vector<std::shared_ptr<Layer>>::const_reverse_iterator rend() const
    {
        return m_Layers.rend();
    }
    
    // Layer stack variables
    // ----------------------------------------
    private:
    ///< Set of layers in the stack.
    std::vector<std::shared_ptr<Layer>> m_Layers;
    ///< Insert index (separating the overlays and rendering layers).
    unsigned int m_LayerInsertIndex = 0;
};

} // namespace pixc
