#pragma once

#include "glWrappers/shaders/ComputeShader.h"
#include "glWrappers/Texture.h"
#include "settings/DataStore.h"

typedef struct PostProcSettings_s {
    bool gammaCorrect;
    bool tonemap;

    PostProcSettings_s() {
        gammaCorrect = true;
        tonemap = false;
    }
} PostProcSettings;

class PostProcessor {

private:
    ComputeShader mShader;

public:
    PostProcessor();

    void applyPostProcessing(Texture &source, Texture &target);
    void applyPostProcessing(Texture &source, Texture &target, PostProcSettings settings);

    void buildImgui(PostProcSettings &postProcSettings, DataStore &settings);
private:
    unsigned int getSettingsBitFlag(PostProcSettings settings);

};
