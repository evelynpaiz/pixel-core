#include "pixcpch.h"
#include "Foundation/Layer/Gui/GuiLayer.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include "Foundation/Core/Application.h"
#include "Foundation/Renderer/Renderer.h"

namespace pixc {

/**
 * @brief Define a GUI layer.
 *
 *@param name Name of the GUI layer.
 */
GuiLayer::GuiLayer(const std::string& name) : Layer(name)
{
    // Initialize the platform-specific ImGui backend
    m_Backend = GuiBackend::Create();
}

/**
 * @brief Called when the GUI layer is attached to the application.
 */
void GuiLayer::OnAttach()
{
    // Set the imgui context
    IMGUI_CHECKVERSION();
    m_GuiContext = ImGui::CreateContext();
    
    // Define imgui flags
    SetGeneralFlags();
    // Set the style of the graphics interface
    SetStyle();
    
    // Initialize the graphics backend
    m_Backend->Init();
}

/**
 * @brief Called when the GUI layer is detached from the application.
 */
void GuiLayer::OnDetach()
{
    // Shut down the appropriate ImGui backend based on the active graphics API
    m_Backend->Shutdown();
    
    // Shut down the GLFW platform backend and destroy the ImGui context
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/**
 * @brief Renders the GUI interface.
 *
 * @param deltaTime Time elapsed since the last frame, in seconds.
 */
void GuiLayer::OnUpdate(Timestep ts)
{
    // Render a simple GUI showing rendering stats
    Begin();
    GUIStats(ts);
    End();
}

/**
 * @brief Handle an event that possibly occurred inside the GUI layer.
 *
 * @param e Event.
 */
void GuiLayer::OnEvent(Event& e)
{
    // If event blocking is disabled, exit early and allow other layers to handle the event
    if (!m_BlockEvents)
        return;
        
    // Forward input handling state to ImGui
    ImGuiIO& io = ImGui::GetIO();
    // Mark the event as handled if ImGui wants to capture mouse or keyboard input
    e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}

/**
 * @brief Begin a new rendering frame for the GUI.
 */
void GuiLayer::Begin()
{
    m_Backend->BeginFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * @brief Render the current defined data of the GUI in the current frame.
 */
void GuiLayer::End()
{
    // Define the application information
    Application &app = Application::Get();
    
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    
    // Render
    ImGui::Render();
    m_Backend->EndFrame();
    
    // Update the context used to rendered
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backupCurrentContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupCurrentContext);
    }
}

/**
 * @brief Render the rendering statistics.
 */
void GuiLayer::GUIStats(Timestep ts)
{
    auto stats = Renderer::GetStats();
    
    ImGui::Begin("Rendering Statistics");
    
    // Define the current window
    Application &app = Application::Get();
    
    ImGui::Text("Resolution: %d x %d", app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    ImGui::Separator();
    ImGui::Text("FPS: %d", ts.GetFPS());
    ImGui::Text("Time (ms) %.2f", ts.GetMilliseconds());
    ImGui::Separator();
    ImGui::Text("Render Passes: %d", stats.renderPasses);
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    
    ImGui::End();
}

/**
 * @brief Check if the GUI is currently active or hovered in.
 *
 * @return `true` if the GUI is active.
 */
bool GuiLayer::IsActive()
{
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) ||
           ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) ||
           ImGui::IsItemActive();
}

/**
 * @brief Define the general flags of imgui.
 */
void GuiLayer::SetGeneralFlags()
{
    ImGuiIO &io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // enable multi-viewport
}

/**
 * @brief Define the style of the GUI.
 */
void GuiLayer::SetStyle()
{
    ImGuiIO &io = ImGui::GetIO();
    
    // When viewports are enabled, tweak window rounding and window background
    // so platform windows can look identical to regular ones
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

} // namespace pixc
