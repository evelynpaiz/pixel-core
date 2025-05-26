#include "enginepch.h"
#include "Common/Layer/GuiLayer.h"

#include "Common/Core/Application.h"
#include "Common/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

using namespace pixc;

/**
 * Define a GUI layer.
 */
GuiLayer::GuiLayer(const std::string& name)
    : Layer(name)
{}

/**
 * Attach (add) the GUI layer to the rendering engine.
 */
void GuiLayer::OnAttach()
{
    // Set the imgui context
    IMGUI_CHECKVERSION();
    m_GuiContext = ImGui::CreateContext();
    
    // Define imgui flags
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // enable multi-viewport
    
    // Define the current window
    Application &app = Application::Get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    
    // Set the style of the graphics interface
    SetStyle();
    
    // Initialize
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

/**
 * Detach (remove) the GUI layer from the rendering engine.
 */
void GuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/**
 * Handle an event that possibly occurred inside the GUI layer.
 * 
 * @param e Event.
 */
void GuiLayer::OnEvent(Event& e)
{
    // Do not handle the event if there is no blocking of the dispatchment
    // to other layers
    if (!m_BlockEvents)
        return;
    
    // Handle the event
    ImGuiIO& io = ImGui::GetIO();
    e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
    e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
}

/**
 * Begin a new rendering frame for the GUI.
 */
void GuiLayer::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * Render the current defined data of the GUI in the current frame.
 */
void GuiLayer::End()
{
    // Define the application information
    Application &app = Application::Get();
    
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    
    // Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
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
 * Render the rendering statistics.
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
 * Define the style of the GUI.
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
