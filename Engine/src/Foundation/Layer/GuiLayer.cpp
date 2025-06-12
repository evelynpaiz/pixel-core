#include "enginepch.h"
#include "Foundation/Layer/GuiLayer.h"

#include "Foundation/Core/Application.h"
#include "Foundation/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// TODO: Add support for metal using a graphic interphase layer.
//#ifdef __APPLE__
//#define IMGUI_IMPL_METAL_CPP
//#include <backends/imgui_impl_metal.h>

// TODO: Test that metal cpp is properly working using:
//#define NS_PRIVATE_IMPLEMENTATION
//#define MTL_PRIVATE_IMPLEMENTATION
//#include "Metal/Metal.hpp"
//#endif

namespace pixc {

/**
 * @brief Define a GUI layer.
 */
GuiLayer::GuiLayer(const std::string& name)
: Layer(name)
{}

/**
 * @brief Attach (add) the GUI layer to the rendering engine.
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
    
    // Initialize based on the graphics API
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            break;
        case RendererAPI::API::OpenGL:
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330");
            break;
        case RendererAPI::API::Metal:
            break;
        default:
            CORE_ASSERT(false, "Unknown Renderer API!");
    }
}

/**
 * @brief Detach (remove) the GUI layer from the rendering engine.
 */
void GuiLayer::OnDetach()
{
    // Shut down the appropriate ImGui backend based on the active graphics API
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            break;
        case RendererAPI::API::OpenGL:
            ImGui_ImplOpenGL3_Shutdown();
            break;
        case RendererAPI::API::Metal:
            //ImGui_ImplMetal_Shutdown();
            break;
        default:
            CORE_ASSERT(false, "Unknown Renderer API!");
    }
    
    // Shut down the GLFW platform backend and destroy the ImGui context
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/**
 * @brief Handle an event that possibly occurred inside the GUI layer.
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
 * @brief Render the GUI layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void GuiLayer::OnUpdate(Timestep ts)
{
    // Render a simple GUI showing rendering stats
    Begin();
    GUIStats(ts);
    End();
}

/**
 * @brief Begin a new rendering frame for the GUI.
 */
void GuiLayer::Begin()
{
    switch (Renderer::GetAPI())
    {
        // Prepare a new ImGui frame based on the active graphics API
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            break;
        case RendererAPI::API::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
        case RendererAPI::API::Metal:
            // TODO: send the render pass descriptor
            //ImGui_ImplMetal_NewFrame();
            break;
        default:
            CORE_ASSERT(false, "Unknown Renderer API!");
    }
    
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
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is not supported!");
            break;
        case RendererAPI::API::OpenGL:
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;
        case RendererAPI::API::Metal:
            break;
        default:
            CORE_ASSERT(false, "Unknown Renderer API!");
    }
    
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
