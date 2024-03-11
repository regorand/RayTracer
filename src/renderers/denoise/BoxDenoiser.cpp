
#include "BoxDenoiser.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"

BoxDenoiser::BoxDenoiser()
    : Denoiser("res/shaders/postProcessing/DenoiserBoxFilter.comp")
{ }

std::string BoxDenoiser::getName() {
    return "Box Filter";
}

void BoxDenoiser::buildImgui(DataStore &settings) {
    ImGui::Separator();
    ImGui::Text("Box Filter");
    ImGui::SliderInt("Denoise Half-KernelSize", settings.getPointer<int>(DENOISE_KERNEL_SIZE), 0, 10);
    ImGui::Separator();
}
