#pragma once

#include "imgui.h"

typedef struct ImGuiMenuSettings_s{
    bool viewPathTraceOutput = false;

    bool showStatsWindow;
    bool showControlPanel;

    ImVec2 menuBarSize;

    ImGuiMenuSettings_s() {
        showStatsWindow = true;
        showControlPanel = true;

        menuBarSize = ImVec2(0, 0);
    }
} ImGuiMenuSettings;


void buildImguiMenu(ImGuiMenuSettings &settings);

void buildImguiAppStats(ImGuiMenuSettings &settings);