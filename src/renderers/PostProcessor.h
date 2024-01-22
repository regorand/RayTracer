#pragma once

#include "glWrappers/shaders/ComputeShader.h"
#include "glWrappers/Texture.h"
#include "dataStructures//DataStore.h"
#include "gui/ImGuiSidebarTab.h"

typedef struct PostProcSettings_s {
    bool gammaCorrect;
    bool tonemap;

    PostProcSettings_s() {
        gammaCorrect = true;
        tonemap = false;
    }
} PostProcSettings;

class PostProcessor : public ImGuiSidebarTab {

private:
    ComputeShader mShader;

public:
    PostProcessor();

    void applyPostProcessing(Texture &source, Texture &target, DataStore &settings);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

private:
    unsigned int getSettingsBitFlag(DataStore &settings);

};
