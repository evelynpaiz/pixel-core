#pragma once

#import "Foundation/Core/Base.h"
#import "Foundation/Layer/Gui/GuiBackend.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief ImGui backend implementation for the OpenGL rendering API.
 *
 * The `OpenGLGuiBackend` class provides the concrete implementation of the
 * `GuiBackend` interface using the OpenGL3 and GLFW backends provided by
 * ImGui. It is responsible for initializing, managing, and rendering ImGui frames in
 * an OpenGL rendering context.
 *
 * The backend is automatically selected at runtime based on the active Renderer API.
 */
class OpenGLGuiBackend : public GuiBackend
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Default constructor.
    OpenGLGuiBackend() = default;
    /// @brief Default destructor.
    virtual ~OpenGLGuiBackend() = default;
    
    virtual void Init() override;
    virtual void Shutdown() override;
    
    // Rendering
    // ----------------------------------------
    virtual void BeginFrame() override;
    virtual void EndFrame() override;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(OpenGLGuiBackend);
};

} // namespace pixc
