#include "ImguiUtils.h"
#include "imgui.h"


void buildImguiMenu(ImGuiMenuSettings &settings) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("TODO: Load Scene"));
            if (ImGui::MenuItem("TODO: Save Image"));
            if (ImGui::MenuItem("TODO: Save Image"));
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            if (ImGui::MenuItem("App Statistics")) {
                // TODO change message dynamically
                settings.showStatsWindow = true;
            }
            if (ImGui::MenuItem("Control Panel")) {
                settings.showGeneralControlPanel = true;
            }
            if (ImGui::MenuItem("Pathtracer Control Panel")) {
                settings.showPTControlPanel = true;
            }
            if (ImGui::MenuItem("Scene Control Panel")) {
                settings.showSceneControlPanel = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void buildImguiAppStats(ImGuiMenuSettings &settings) {
    ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_Once);
    ImGui::Begin("Application Stats", &settings.showStatsWindow,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMouseInputs
                 | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}