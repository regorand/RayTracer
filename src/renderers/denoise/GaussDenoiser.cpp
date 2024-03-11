
#include <iostream>
#include "GaussDenoiser.h"
#include "dataStructures/SettingIndex.h"
#include "imgui.h"

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
