#include "Denoiser.h"
#include "deviceShared/BufferBindings.h"
#include "dataStructures/SettingIndex.h"

void Denoiser::performDenoise(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    performDenoiseIteration(buffers, textureStore, settings);
}

void Denoiser::performDenoiseIteration(TmpBuffers &buffers, std::map<int, Texture> &textureStore, DataStore &settings) {
    bindResources(buffers, textureStore, settings);

    glDispatchCompute(buffers.tmp1.getWidth(), buffers.tmp1.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    unBindResources(buffers, textureStore, settings);
}

void Denoiser::reloadShader() {
    mShader.reloadShader();
}
