#pragma once

#include "Foundation/Core/ClassUtils.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Abstract base class for platform-specific ImGui rendering backends.
 *
 * The `GuiBackend` class defines the interface for ImGui backend implementations that are
 * responsible for initializing, updating, and rendering ImGui using a specific
 * graphics API (e.g., OpenGL, Metal).
 *
 * Derived classes must implement all pure virtual methods to handle backend-specific
 * logic. This abstraction allows the GUI system to remain decoupled from any
 * particular rendering API.
 */
class GuiBackend
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Default destructor.
    virtual ~GuiBackend() = default;
    
    // Initialization
    // ----------------------------------------
    static std::unique_ptr<GuiBackend> Create();
    virtual void Init() = 0;
    
    // Initialization
    // ----------------------------------------
    virtual void Shutdown() = 0;
    
    // Rendering
    // ----------------------------------------
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    
protected:
    // Constructor
    // ----------------------------------------
    /// @brief Default constructor.
    GuiBackend() = default;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(GuiBackend);
};

} // namespace pixc
