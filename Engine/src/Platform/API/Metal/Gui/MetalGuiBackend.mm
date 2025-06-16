#include "enginepch.h"
#include "Platform/Metal/Gui/MetalGuiBackend.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include "Foundation/Core/Application.h"
#include "Platform/Metal/MetalContext.h"

namespace pixc {

/**
 * @brief Initializes the ImGui Metal backend.
 */
void MetalGuiBackend::Init()
{
    // Define the current window
    Application &app = Application::Get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    
    // Define the device that is currently in use
    MetalContext* context = dynamic_cast<MetalContext*>(&GraphicsContext::Get());
    PIXEL_CORE_ASSERT(context, "Graphic context is not Metal!");
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(context->GetDevice());
    
    // Initialize
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplMetal_Init(device);
}

/**
 * @brief Shuts down the ImGui Metal backend.
 */
void MetalGuiBackend::Shutdown()
{
    ImGui_ImplMetal_Shutdown();
}

/**
 * @brief Begins a new ImGui frame using the Metal backend.
 */
void MetalGuiBackend::BeginFrame()
{
    // TODO: create a function in context that will give back a generic pass descriptor
    //ImGui_ImplMetal_NewFrame(renderPassDescriptor);
}

/**
 * @brief Renders the ImGui draw data using Metal.
 */
void MetalGuiBackend::EndFrame()
{
    // TODO: get the proper command buffer and render encoder
    //ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);
}

} // namespace pixc
