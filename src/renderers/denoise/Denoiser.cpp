#include "Denoiser.h"
#include "deviceShared/BufferBindings.h"
#include "dataStructures/SettingIndex.h"

void Denoiser::performDenoise(TmpBuffers &buffers, DataStore &settings) {
    performDenoiseIteration(buffers, settings);
}

void Denoiser::performDenoiseIteration(TmpBuffers &buffers, DataStore &settings) {
    auto &source = buffers.getSourceTex();
    auto &target = buffers.getTargetTex();

    source.bindAsImage(POST_PROC_SOURCE_BINDING);
    target.bindAsImage(POST_PROC_TARGET_BINDING);

    mShader.bind();

    int halfKernelSize = settings.getOrElse(DENOISE_KERNEL_SIZE, 0);
    mShader.setUniform1i("halfKernelSize", halfKernelSize);

    glDispatchCompute(source.getWidth(), source.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    mShader.unbind();
    source.unbind();
    target.unbind();
}

void Denoiser::reloadShader() {
    mShader.reloadShader();
}
