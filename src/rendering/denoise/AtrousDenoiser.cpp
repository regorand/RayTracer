
#include "AtrousDenoiser.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"
#include "deviceShared/BufferBindings.h"
#include "dataStructures/TextureNames.h"

AtrousDenoiser::AtrousDenoiser()
    : Denoiser("res/shaders/postProcessing/DenoiserAtrousFilter.comp"),
    currentIteration(0)
{ }



std::string AtrousDenoiser::getName() {
    return DENOISER_NAME;
}

void AtrousDenoiser::buildImgui(DataStore &settings) {
    ImGui::Separator();
    ImGui::Text("Atrous Filter");
    ImGui::SliderInt("Iterations", settings.getPointer<int>(ATROUS_ITERATION_COUNT), 1, 10);
    ImGui::SliderFloat("Color Std Dev", settings.getPointer<float>(ATROUS_COLOR_STD_DEV), 0.1f, 2.0f);
    ImGui::SliderFloat("Pos Std Dev", settings.getPointer<float>(ATROUS_POS_STD_DEV), 0.1f, 2.0f);
    ImGui::SliderFloat("Normal Std Dev", settings.getPointer<float>(ATROUS_NORMAL_STD_DEV), 0.1f, 2.0f);
    ImGui::Separator();
}

void AtrousDenoiser::performDenoise(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    int iterations = settings.getOrElse(ATROUS_ITERATION_COUNT, 1);


    for (int i = 0; i < iterations; i++) {
        currentIteration = i;

        performDenoiseIteration(buffers, textureStore, settings);
    }

    currentIteration = -1;

    mShader.unbind();
}

void AtrousDenoiser::bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    textureStore.at(POS_BUFFER).bindAsImage(RAST_POS_BUFFER_BINDING);
    textureStore.at(NORMAL_BUFFER).bindAsImage(RAST_NORMAL_BUFFER_BINDING);

    mShader.bind();
    mShader.setUniform1i("iteration", currentIteration);

    mShader.setUniform1f("colorStdDev", settings.getOrElse(ATROUS_COLOR_STD_DEV, 1.0f));
    mShader.setUniform1f("posStdDev", settings.getOrElse(ATROUS_POS_STD_DEV, 1.0f));
    mShader.setUniform1f("normalStdDev", settings.getOrElse(ATROUS_NORMAL_STD_DEV, 1.0f));
}

void AtrousDenoiser::unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    source.unbind();
    target.unbind();
    buffers.swap();
}
