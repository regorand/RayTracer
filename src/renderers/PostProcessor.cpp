
#include <iostream>
#include "PostProcessor.h"
#include "deviceShared/PostProcessFlags.h"
#include "deviceShared/BufferBindings.h"
#include "imgui.h"
#include "dataStructures/SettingIndex.h"
#include "renderers/denoise/GaussDenoiser.h"
#include "renderers/denoise/BoxDenoiser.h"
#include "glm/common.hpp"

PostProcessor::PostProcessor(int textureWidth, int textureHeight)
        :
        mResolveSampleCountShader("res/shaders/postProcessing/ResolveSampleCount.comp"),
        mCopyTextureShader("res/shaders/postProcessing/CopyTexture.comp"),
        mGammaTonemapShader("res/shaders/postProcessing/PostProcess.comp"),
        mWorkToDo(false),
        mTmpBuffers(textureWidth, textureHeight)
{
    denoisers.emplace_back(new BoxDenoiser());
    denoisers.emplace_back(new GaussDenoiser());
}

PostProcessor::~PostProcessor() {
    for (auto denoiser: denoisers) {
        delete denoiser;
    }
}

void PostProcessor::applyPostProcessing(Texture &source, Texture &target, DataStore &settings) {
    if (!(mWorkToDo || settings.getOrElse(CONTINUALLY_APPLY_POSTPROC, false))) return;

    mWorkToDo = false;

    if (source.getWidth() != target.getWidth() || source.getHeight() != target.getHeight()) {
        std::cerr << "Can't apply post processing if source and target buffer are not the same size" << std::endl;
    }

    noUniformsOperation(mResolveSampleCountShader, source, mTmpBuffers.getTargetTex());
    mTmpBuffers.swap();

    applyGammaAndTonemap(mTmpBuffers.getSourceTex(), mTmpBuffers.getTargetTex(), settings);
    mTmpBuffers.swap();

    if (settings.getOrElse(DO_DENOISE, false)) {
        auto index = settings.getOrElse<int>(DENOISER_CHOICE_INDEX, 0);
        denoisers[index]->performDenoise(mTmpBuffers, settings);
        mTmpBuffers.swap();
    }


    noUniformsOperation(mCopyTextureShader, mTmpBuffers.getSourceTex(), target);
}


void PostProcessor::noUniformsOperation(ComputeShader shader, Texture &source, Texture &target) {
    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    shader.bind();

    glDispatchCompute(source.getWidth(), source.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    shader.unbind();
    source.unbind();
    target.unbind();
}

void PostProcessor::applyGammaAndTonemap(Texture &source, Texture &target, DataStore &settings) {
    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    mGammaTonemapShader.bind();

    mGammaTonemapShader.setUniform1ui("postProcessFlags", getSettingsBitFlag(settings));

    glDispatchCompute(source.getWidth(), source.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    mGammaTonemapShader.unbind();
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

        ImGui::Checkbox("Continually Apply", settings.getPointer<bool>(CONTINUALLY_APPLY_POSTPROC));

        if (ImGui::Button("Reload post Proc shaders", buttonSize)) {
            for (auto denoiser: denoisers) {
                denoiser->reloadShader();
            }
            mGammaTonemapShader.reloadShader();
        }

        if (ImGui::Button("Apply Post Processing", buttonSize)) {
            applyNextCall();
        }
        ImGui::Separator();

        ImGui::Checkbox("Denoising", settings.getPointer<bool>(DO_DENOISE));

        if (settings.getOrElse(DO_DENOISE, false)) {
            auto items = denoisers;
            int currentIndex = settings.getOrInit<int>(DENOISER_CHOICE_INDEX);
            currentIndex = glm::min(glm::max(currentIndex, 0), (int) items.size() - 1);

            auto selected = items[currentIndex];
            if (ImGui::BeginCombo(" Buffer", items[currentIndex]->getName().c_str())) {
                for (int i = 0; i < items.size(); i++) {
                    bool is_selected = (selected->getName() == items[i]->getName()); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(items[i]->getName().c_str(), is_selected)) {
                        selected = items[i];
                        currentIndex = i;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)

                }

                ImGui::EndCombo();
                settings.store(DENOISER_CHOICE_INDEX, currentIndex);
            }

            selected->buildImgui(settings);
        }

        ImGui::Separator();

        ImGui::Checkbox("Gamma Correction", settings.getPointer<bool>(GAMMA_CORRECT));
        ImGui::Checkbox("tonemapping", settings.getPointer<bool>(TONEMAP));
    }
}

void PostProcessor::applyNextCall() {
    mWorkToDo = true;
}
