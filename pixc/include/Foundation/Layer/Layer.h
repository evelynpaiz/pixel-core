#pragma once

#include "Foundation/Core/ClassUtils.h"

#include "Foundation/Core/Timestep.h"

#include "Foundation/Event/Event.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a layer in a rendering engine.
 *
 * The `Layer` class provides a mechanism for organizing and managing the rendering process in a
 * rendering engine. Layers can be attached and detached, allowing for dynamic management of rendering
 * order. They also provide an interface for updating the layer state and handling events specific to the layer.
 *
 * Copying or moving `Layer` objects is disabled to ensure single ownership and prevent unintended
 * layer duplication.
 */
class Layer
{
    public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Define a rendering layer.
    /// @param name Name of the layer.
    Layer(const std::string& name = "Unidentified Layer")
    : m_LayerName(name)
    {}
    /// @brief Delete the layer.
    virtual ~Layer() = default;
    
    // Layer handlers
    // ----------------------------------------
    /// @brief Attach (add) this layer to the rendering engine.
    virtual void OnAttach() {}
    /// @brief Detach (remove) this layer from the rendering engine.
    virtual void OnDetach() {}
    /// @brief Render this layer.
    /// @param deltaTime Times passed since the last update.
    virtual void OnUpdate(Timestep ts) {}
    /// @brief Handle an event that possibly occurred inside the layer.
    /// @param e Event.
    virtual void OnEvent(Event& e) {}
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the name of the layer.
    /// @return Name of the layer.
    std::string GetName() const { return m_LayerName; }
    
    // Layer variables
    // ----------------------------------------
    protected:
    ///< Layer name.
    std::string m_LayerName;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
    public:
    DISABLE_COPY_AND_MOVE(Layer);
};

} // namespace pixc
