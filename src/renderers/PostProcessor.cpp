
#include <iostream>
#include "PostProcessor.h"
#include "deviceShared/PostProcessFlags.h"
#include "deviceShared/BufferBindings.h"

PostProcessor::PostProcessor()
    : mShader("res/shaders/postProcessing/PostProcess.comp")
{ }

void PostProcessor::applyPostProcessing(Texture &source, Texture &target) {
    applyPostProcessing(source, target, PostProcSettings());
}

void PostProcessor::applyPostProcessing(Texture &source, Texture &target, PostProcSettings settings) {
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

unsigned int PostProcessor::getSettingsBitFlag(PostProcSettings settings) {
    unsigned int val = 0;
    if (settings.gammaCorrect) val += GAMMA_CORRECT;
    if (settings.tonemap) val += TONEMAP;

    return val;
}
