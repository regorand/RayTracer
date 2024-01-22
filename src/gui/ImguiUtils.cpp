#include "ImguiUtils.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"

void buildImguiMenu(DataStore &settings) {
    if (ImGui::BeginMainMenuBar()) {
        settings.store(SettingIndex::MENU_BAR_SIZE, ImVec2ToGlmVec2(ImGui::GetWindowSize()));
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("TODO: Load Scene"));
            if (ImGui::MenuItem("TODO: Save Image"));
            if (ImGui::MenuItem("TODO: Save Image"));
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            // TODO change message dynamically

            if (ImGui::MenuItem("App Statistics")) {
                settings.store(SHOW_STATS_WINDOW, !settings.getOrElse(SHOW_STATS_WINDOW, true));
            }

            if (ImGui::MenuItem("Control Panel")) {
                settings.store(SHOW_CONTROL_PANEL, !settings.getOrElse(SHOW_CONTROL_PANEL, true));
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void buildImguiAppStats(DataStore &settings) {
    ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_Once);
    ImGui::Begin("Application Stats", settings.getPointer<bool>(SHOW_STATS_WINDOW),
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMouseInputs
                 | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

glm::vec2 ImVec2ToGlmVec2(ImVec2 vec) {
    return {vec.x, vec.y};
}

void startIndent() {
    ImGui::Indent(INDENT_DEPTH);
}

void endIndent() {
    ImGui::Unindent(INDENT_DEPTH);
}

void spacingAndSeperator() {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void endAndNewIndent() {
    startIndent();
    spacingAndSeperator();
    endIndent();
}
