#pragma once

#include "Common/Core/FileDialogs.h"

#include "Common/Renderer/Texture/Texture2D.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace utils { namespace Draw {

/**
 * Display read-only text in an input field.
 *
 * @param id  A unique identifier for the input field.
 * @param text The text to display in the input field.
 */
inline void TextDisplay(const char *id, const char *text)
{
    ImGui::PushID(id);
    char buffer[256];
    strcpy(buffer, text);
    ImGui::PushItemWidth(-1);
    ImGui::InputText(
            "##Text",
            buffer,
            sizeof(buffer),
            ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
    ImGui::PopID();
}

/**
 * Controller button.
 *
 * @param label Name.
 * @param value Values to be displayed.
 * @param buttonSize Size of the button.
 * @param resetValue Reset values to be displayed.
 * @param defaultColor Color of the button.
 * @param hoverColor Color when the button is hovered.
 * @param activeColor Color when the button is active.
 * @param boldFont Pointer to the bold font.
 */
inline void Control(const std::string &label, float &value,
                    const ImVec2 &buttonSize, const float &resetValue = 0.0f,
                    const ImVec4 &defaultColor = ImVec4(0.14f, 0.14f, 0.14f, 1.0f),
                    const ImVec4 &hoverColor = ImVec4(1.0f, 1.0f, 1.0f, 0.156f),
                    const ImVec4 &activeColor = ImVec4(1.0f, 1.0f, 1.0f, 0.391f),
                    ImFont *boldFont = nullptr)
{
    // Define the style of the button
    ImGui::PushStyleColor(ImGuiCol_Button, defaultColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
    if (boldFont)
        ImGui::PushFont(boldFont);
    
    // Display the control button
    if (ImGui::Button(label.c_str(), buttonSize))
        value = resetValue;
    
    // Pop the style defined previusly
    if (boldFont)
        ImGui::PopFont();
    ImGui::PopStyleColor(3);
    
    // Allow to change the values by dragging
    ImGui::SameLine();
    
    ImGui::DragFloat(("##" + label).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f");
}

/**
 * Vec3 controller.
 *
 * @param label Name.
 * @param value Values to be displayed.
 * @param resetValue Reset values to be displayed.
 * @param columnWidth Size of the control.
 */
inline void Vec3Control(const std::string &label,
                        glm::vec3 &value,
                        const float &resetValue = 0.0f,
                        const float &columnWidth = 120.0f)
{
    ImGuiIO &io = ImGui::GetIO();
    auto *boldFont = io.Fonts->Fonts[0];
    
    ImGui::PushID(label.c_str());
    
    // Separate in two colums and display the
    // label in the first one
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();
    
    // Calculate the remaining width
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 0, 0 });
    
    float lineHeight
    = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
    
    // Display the controls (x, y, z)
    Control("X", value.x,
            buttonSize, resetValue,
            ImVec4(0.8f, 0.1f, 0.15f, 1.0f),
            ImVec4(0.9f, 0.2f, 0.2f, 1.0f),
            ImVec4(0.8f, 0.1f, 0.15f, 1.0f),
            boldFont);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    Control("Y", value.y,
            buttonSize, resetValue,
            ImVec4(0.2f, 0.7f, 0.2f, 1.0f),
            ImVec4(0.3f, 0.8f, 0.3f, 1.0f),
            ImVec4(0.2f, 0.7f, 0.2f, 1.0f),
            boldFont);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    Control("Z",
            value.z,
            buttonSize,
            resetValue,
            ImVec4(0.1f, 0.25f, 0.8f, 1.0f),
            ImVec4(0.2f, 0.35f, 0.9f, 1.0f),
            ImVec4(0.1f, 0.25f, 0.8f, 1.0f),
            boldFont);
    ImGui::PopItemWidth();
    // Reset
    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();
}

/**
 * Load and display an image texture using ImGui.
 *
 * @param texture  A reference to a shared pointer to a texture object. The loaded texture
 *                 will be stored here.
 * @param filePath A reference to the path where the loaded image file's path will be stored.
 * @param label    The label text displayed on the load button.
 * @param filter   A pointer to a filter string specifying the allowed file types for the image files.
 * @param flip     A boolean flag indicating whether to flip the image vertically when displayed.
 *
 * @return `true` if the texture has been updated (new image loaded), `false` otherwise.
 */
inline bool TextureLoader(std::shared_ptr<Texture2D> &texture,
                          std::filesystem::path &filePath, const char *label,
                          const char *filter, const bool &flip = true)
{
    CORE_ASSERT(false, "Function needs to be define to support Metal");
    /*
    // Define result (texture has been updated)
    bool updated = false;
    
    // Select the correct texture to be displayed
    const auto &displayTexture = texture ? texture : utils::textures::EmptyTexture2D();

    ImTextureID textureID = (ImTextureID)((uintptr_t)(displayTexture->m_ID));
    float aspect = (float)displayTexture->m_Spec.Height / displayTexture->m_Spec.Width;
    
    // Display the texture as an image button
    ImGui::Columns(2);
    
    ImGui::SetColumnWidth(0, 92.f);
    if (ImGui::ImageButton(
            textureID,
            ImVec2 { 70, 70 * aspect },
            flip ? ImVec2 { 0, 1 } : ImVec2 { 1, 0 },
            flip ? ImVec2 { 1, 0 } : ImVec2 { 0, 1 }))
    {
        filePath = FileDialogs::OpenFile(filter);
        if (!filePath.empty())
        {
            texture = Texture2D::CreateFromFile(filePath);
            updated = true;
        }
    }
    
    ImGui::NextColumn();
    
    // Texture name
    ImGui::Text("%s", label);
    ImGui::SameLine();
    TextDisplay(label, filePath.filename().string().c_str());

    if (ImGui::Button("Clear"))
        texture = nullptr;
    ImGui::Columns(1);
    
    return updated;
     */
}
    
} // namespace Draw
} // namespace utils
