#pragma once

#include "glWrappers/shaders/ComputeShader.h"
#include "glWrappers/Texture.h"
#include "dataStructures//DataStore.h"
#include "gui/ImGuiSidebarTab.h"
#include "TmpBuffers.h"
#include "renderers/denoise/Denoiser.h"
#include "renderers/denoise/AtrousDenoiser.h"

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
    ComputeShader mResolveSampleCountShader;
    ComputeShader mCopyTextureShader;
    ComputeShader mGammaTonemapShader;

    std::vector<Denoiser *> denoisers;

    bool mWorkToDo;
    TmpBuffers mTmpBuffers;

public:
    PostProcessor(int textureWidth, int textureHeight);
    ~PostProcessor();

    void applyPostProcessing(Texture &source, Texture &target, std::map<int, Texture> textureStore, DataStore &settings);

    void noUniformsOperation(ComputeShader shader, Texture &source, Texture &target);
    void applyGammaAndTonemap(Texture &source, Texture &target, DataStore &settings);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

    void applyNextCall();

private:
    unsigned int getSettingsBitFlag(DataStore &settings);

};
