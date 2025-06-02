#include "Viewer/ViewerGui.h"

#include <imgui.h>

/**
 * @brief Define a Viewer GUI layer.
 */
ViewerGui::ViewerGui(const std::shared_ptr<Viewer>& layer)
    : GuiLayer("GUI Layer"), m_Viewer(layer)
{}

/**
 * @brief Render the GUI layer.
 *
 * @param deltaTime Times passed since the last update.
 */
void ViewerGui::OnUpdate(Timestep ts)
{
    // Render GUI
    Begin();
    GUIStats(ts);
    GUIMenu();
    End();
    
    // TODO: incorporate into a RENDERING LAYER
    // If GUI is active, disable the interaction in the rendering layer
    //m_Viewer->EnableInteraction(!IsActive());
}

/**
 * @brief Handle an event that possibly occurred inside this layer.
 * @param e Event.
 */
void ViewerGui::OnEvent(Event &e)
{
    // Define the event dispatcher
    EventDispatcher dispatcher(e);
    
    // Dispatch the event to the application event callbacks
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(ViewerGui::OnMouseScrolled));
}

/**
 * @brief Check if the GUI is currently active or hovered in.
 *
 * @return `true` if the GUI is active.
 */
bool ViewerGui::IsActive()
{
    return ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)
    || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
    || ImGui::IsItemActive();
}

/**
 * @brief The viewer menu GUI.
 */
void ViewerGui::GUIMenu()
{
    /*
    // Render the menu
    ImGui::Begin("3D Viewer");
    
    // Define the variables
    glm::vec3 lightPosition = m_Viewer->GetLightSource()->GetPosition();
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
        utils::Draw::Vec3Control("Position", lightPosition);
    }
    // Update
    m_Viewer->GetLightSource()->SetPosition(lightPosition);
    
    ImGui::End();
     */
}

/**
 * @brief Define the style of the GUI.
 */
void ViewerGui::SetStyle()
{
    // Set the base style of the GUI
    GuiLayer::SetStyle();
    
    // Define the lighter color
    ImGui::StyleColorsDark();
    
    // Define colors specifically
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.93f, 0.90f, 0.92f, 1.00f);                 // Text color
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.59f, 0.60f, 1.00f);         // Disabled text color
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.23f, 0.23f, 0.25f, 0.94f);             // Window background color
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.26f, 0.30f, 0.00f);              // Child background color
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.25f, 0.29f, 0.94f);              // Popup background color
    style.Colors[ImGuiCol_Border] = ImVec4(0.29f, 0.29f, 0.32f, 0.50f);               // Border color
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);         // Border shadow color
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.34f, 0.31f, 0.36f, 0.54f);              // Frame background color
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.41f, 0.38f, 0.43f, 0.60f);       // Frame background color when hovered
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.47f, 0.43f, 0.49f, 0.67f);        // Frame background color when active
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.78f, 0.35f, 0.60f, 0.45f);              // Title background color
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.84f, 0.38f, 0.64f, 0.68f);        // Title background color when active
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.84f, 0.38f, 0.64f, 0.45f);     // Title background color when collapsed
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.30f, 0.27f, 0.31f, 0.57f);            // Menu bar background color
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.23f, 0.26f, 0.66f);          // Scrollbar background color
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.59f, 0.27f, 0.50f, 0.51f);        // Scrollbar grab color
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.68f, 0.31f, 0.57f, 0.78f); // Scrollbar grab color when hovered
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.78f, 0.35f, 0.60f, 1.00f);  // Scrollbar grab color when active
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.35f, 0.60f, 0.80f);            // Checkmark color
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.78f, 0.35f, 0.60f, 0.50f);           // Slider grab color
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.38f, 0.64f, 1.00f);     // Slider grab color when active
    style.Colors[ImGuiCol_Button] = ImVec4(0.78f, 0.35f, 0.60f, 0.40f);               // Button color
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.84f, 0.38f, 0.64f, 0.60f);        // Button color when hovered
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.84f, 0.38f, 0.64f, 0.67f);         // Button color when active
    style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.30f, 0.55f);               // Header color
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);        // Header color when hovered
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);         // Header color when active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.29f, 0.29f, 0.32f, 0.50f);            // Separator color
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.38f, 0.43f, 0.78f);     // Separator color when hovered
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.47f, 0.43f, 0.49f, 0.67f);      // Separator color when active
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);           // Resize grip color
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.38f, 0.64f, 0.67f);    // Resize grip color when hovered
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.38f, 0.64f, 0.95f);     // Resize grip color when active
    style.Colors[ImGuiCol_Tab] = ImVec4(0.23f, 0.23f, 0.25f, 0.86f);                  // Tab color
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.78f, 0.35f, 0.60f, 0.80f);           // Tab color when hovered
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.35f, 0.60f, 0.67f);            // Tab color when active
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.23f, 0.23f, 0.25f, 0.94f);         // Tab color when unfocused
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.34f, 0.31f, 0.36f, 0.67f);   // Tab color when unfocused and active
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.78f, 0.35f, 0.60f, 1.00f);            // Plot lines color
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.84f, 0.38f, 0.64f, 1.00f);     // Plot lines color when hovered
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.78f, 0.35f, 0.60f, 1.00f);        // Plot histogram color
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.84f, 0.38f, 0.64f, 1.00f); // Plot histogram color when hovered
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.84f, 0.38f, 0.64f, 0.35f);       // Selected text background color
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.84f, 0.38f, 0.64f, 0.95f);       // Drag and drop target color
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.78f, 0.35f, 0.60f, 1.00f);         // Navigation highlight color
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.78f, 0.35f, 0.60f, 0.70f);// Navigation window highlight color
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);    // Navigation window dim background color
}

/**
 * @brief Function to be called when a mouse scroll event happens.
 *
 * @param e Event to be handled.
 * @return `true` if the event has been handled.
 */
bool ViewerGui::OnMouseScrolled(MouseScrolledEvent &e)
{
    return IsActive();
}
