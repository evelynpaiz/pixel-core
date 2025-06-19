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
#include "Platform/Metal/MetalRendererAPI.h"

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
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    PIXEL_CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
    
    // Get the Metal device from the context
    id<MTLDevice> device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
    
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
    // Define command buffer if necessary
    bool clear = m_Context->InitCommandBuffer();
    
    // Get the drawable
    auto drawable = reinterpret_cast<id<CAMetalDrawable>>(m_Context->GetDrawable());
    
    // Define render pass descriptor
    MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
    descriptor.colorAttachments[0].texture = drawable.texture;
    descriptor.colorAttachments[0].loadAction = clear ? MTLLoadActionClear : MTLLoadActionLoad;
    descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    
    // Define render encoder
    m_Context->InitCommandEncoder(descriptor, "ImGui");
    // Define the imgui frame
    ImGui_ImplMetal_NewFrame(descriptor);
}

/**
 * @brief Renders the ImGui draw data using Metal.
 */
void MetalGuiBackend::EndFrame()
{
    // Get the command buffer and encoder used to render the frame
    auto commandBuffer = reinterpret_cast<id<MTLCommandBuffer>>(m_Context->GetCommandBuffer());
    auto encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());
    
    // Render the imgui data
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, encoder);
}

} // namespace pixc
