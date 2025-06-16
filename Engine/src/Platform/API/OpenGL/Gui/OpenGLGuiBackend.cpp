#include "enginepch.h"
#include "Platform/OpenGL/Gui/OpenGLGuiBackend.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Foundation/Core/Application.h"

namespace pixc {

/**
 * @brief Initializes the ImGui OpenGL backend.
 */
void OpenGLGuiBackend::Init()
{
    // Define the current window
    Application &app = Application::Get();
    GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    
    // Initialize
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

/**
 * @brief Shuts down the ImGui OpenGL backend.
 */
void OpenGLGuiBackend::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
}

/**
 * @brief Begins a new ImGui frame using the OpenGL backend.
 */
void OpenGLGuiBackend::BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

/**
 * @brief Renders the ImGui draw data using OpenGL.
 */
void OpenGLGuiBackend::EndFrame()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace pixc
