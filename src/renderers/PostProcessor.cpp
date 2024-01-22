
#include <iostream>
#include "PostProcessor.h"
#include "deviceShared/PostProcessFlags.h"
#include "deviceShared/BufferBindings.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"

PostProcessor::PostProcessor()
        : mShader("res/shaders/postProcessing/PostProcess.comp") {}

void PostProcessor::applyPostProcessing(Texture &source, Texture &target, DataStore &settings) {
    if (source.getWidth() != target.getWidth() || source.getHeight() != target.getHeight()) {
        std::cerr << "Can't apply post processing if source and target buffer are not the same size" << std::endl;
    }

    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    mShader.bind();

    mShader.setUniform1ui("postProcessFlags", getSettingsBitFlag(settings));

    glDispatchCompute(source.getWidth(), source.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    mShader.unbind();
    source.unbind();
    target.unbind();
}

unsigned int PostProcessor::getSettingsBitFlag(DataStore &settings) {
    unsigned int val = 0;
    if (settings.getOrElse(GAMMA_CORRECT, false)) val += GAMMA_CORRECT_BIT_FLAG;
    if (settings.getOrElse(TONEMAP, false)) val += TONEMAP_BIT_FLAG;

    return val;
}

void PostProcessor::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Post Processor")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reload post Proc shaders", buttonSize)) {
            mShader.reloadShader();
        }

        ImGui::Checkbox("Gamma Correction", settings.getPointer<bool>(GAMMA_CORRECT));
        ImGui::Checkbox("tonemapping", settings.getPointer<bool>(TONEMAP));
    }
}
