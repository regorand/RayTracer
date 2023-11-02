#pragma once

typedef struct ImGuiMenuSettings_s{
    bool showStatsWindow;
    bool showGeneralControlPanel;
    bool showPTControlPanel;
    bool showSceneControlPanel;

    ImGuiMenuSettings_s() {
        showStatsWindow = true;
        showGeneralControlPanel = false;
        showPTControlPanel = false;
        showSceneControlPanel = false;
    }
} ImGuiMenuSettings;


void buildImguiMenu(ImGuiMenuSettings &settings);

void buildImguiAppStats(ImGuiMenuSettings &settings);