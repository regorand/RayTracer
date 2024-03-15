
#include "BoxDenoiser.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"
#include "deviceShared/BufferBindings.h"

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

void BoxDenoiser::bindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    mShader.bind();

    int halfKernelSize = settings.getOrElse(DENOISE_KERNEL_SIZE, 0);
    mShader.setUniform1i("halfKernelSize", halfKernelSize);
}

void BoxDenoiser::unBindResources(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    mShader.unbind();
    source.unbind();
    target.unbind();
}
