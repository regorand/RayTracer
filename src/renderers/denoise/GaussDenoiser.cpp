
#include <iostream>
#include "GaussDenoiser.h"
#include "dataStructures/SettingIndex.h"
#include "imgui.h"
#include "deviceShared/BufferBindings.h"

GaussDenoiser::GaussDenoiser()
    : Denoiser("res/shaders/postProcessing/DenoiserGaussFilter.comp")
{}

std::string GaussDenoiser::getName() {
    return DENOISER_NAME;
}

void GaussDenoiser::buildImgui(DataStore &settings) {
    ImGui::Separator();
    ImGui::SliderInt("Denoise Half-KernelSize", settings.getPointer<int>(DENOISE_KERNEL_SIZE), 0, 10);
    ImGui::Separator();
}

void GaussDenoiser::bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    mShader.bind();

    int halfKernelSize = settings.getOrElse(DENOISE_KERNEL_SIZE, 0);
    mShader.setUniform1i("halfKernelSize", halfKernelSize);
}

void GaussDenoiser::unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    mShader.unbind();
    source.unbind();
    target.unbind();
}
