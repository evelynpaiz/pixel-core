#pragma once

#import "Foundation/Layer/Gui/GuiBackend.h"

#include "Platform/Metal/MetalContext.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief ImGui backend implementation for the Metal rendering API.
 *
 * The `MetalGuiBackend` class provides the concrete implementation of the
 * `GuiBackend` interface using the Metal and GLFW backends provided by ImGui.
 * It is responsible for initializing, managing, and rendering ImGui frames in an Metal
 * rendering context.
 *
 * The backend is automatically selected at runtime based on the active Renderer API.
 *
 * @see GuiBackend
 */
class MetalGuiBackend : public GuiBackend
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Default constructor.
    MetalGuiBackend() = default;
    /// @brief Default destructor.
    virtual ~MetalGuiBackend() = default;
    
    virtual void Init() override;
    virtual void Shutdown() override;
    
    // Rendering
    // ----------------------------------------
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalGuiBackend);
    
    // Backend variables
    // ----------------------------------------
private:
    ///< Metal context.
    MetalContext* m_Context;
};

} // namespace pixc
