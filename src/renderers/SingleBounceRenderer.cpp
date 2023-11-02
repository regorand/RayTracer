
#include <random>
#include <ctime>
#include "SingleBounceRenderer.h"
#include "imgui.h"

SingleBounceRenderer::SingleBounceRenderer(int width, int height)
        : mWidth(width),
          mHeight(height),
        // mPosBuffer(TextureBuffer(nullptr, mWidth * mHeight * sizeof(float) * 4, POS_BUFFER_TARGET, POS_BUFFER_FORMAT, GL_DYNAMIC_COPY)),
        // mDirBuffer(TextureBuffer(nullptr, mWidth * mHeight * sizeof(float) * 4, DIR_BUFFER_TARGET, DIR_BUFFER_FORMAT, GL_DYNAMIC_COPY)),
        // mMultiplierBuffer(TextureBuffer(nullptr, mWidth * mHeight * sizeof(float) * 4, MULT_BUFFER_TARGET, MULT_BUFFER_FORMAT, GL_DYNAMIC_COPY)),
          mPosBuffer(Texture(new HostBuffer(mWidth, mHeight))),
          mDirBuffer(Texture(new HostBuffer(mWidth, mHeight))),
          mMultiplierBuffer(Texture(new HostBuffer(mWidth, mHeight))),

          mShader(ComputeShader("res/shaders/compute/computeSingleBounce.comp")),
          mNewPathShader(ComputeShader("res/shaders/compute/CreateNewPath.comp")),
          mClearTargetShader(ComputeShader("res/shaders/compute/clearColorBuffer.comp")) {
    newPath();
}

void SingleBounceRenderer::render(Texture *target, RenderSettings renderSettings) {
    newPath();

    target->bindAsImage(TARGET_BUFFER_TARGET);

    mPosBuffer.bindAsImage(POS_BUFFER_TARGET);
    mDirBuffer.bindAsImage(DIR_BUFFER_TARGET);
    mMultiplierBuffer.bindAsImage(MULT_BUFFER_TARGET);

    mShader.bind();

    if (renderSettings.russianRoulette) {
        while (dist.sampleFloat() < renderSettings.alphaRR) {
            renderSinglePass();
        }
    } else {
        for (int i = 0; i < renderSettings.constantPathLength; i++) {
            renderSinglePass();
        }
    }
}

void SingleBounceRenderer::reloadShader() {
    mShader.reloadShader();
    mNewPathShader.reloadShader();
    mClearTargetShader.reloadShader();
}

void SingleBounceRenderer::renderSinglePass() {
    glm::vec2 seed = dist.sampleVec2();

    mShader.setUniform2f("frameSeed", seed);

    glDispatchCompute(mWidth, mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void SingleBounceRenderer::newPath() {

    glm::vec2 seed = dist.sampleVec2();

    mPosBuffer.bindAsImage(POS_BUFFER_TARGET);
    mDirBuffer.bindAsImage(DIR_BUFFER_TARGET);
    mMultiplierBuffer.bindAsImage(MULT_BUFFER_TARGET);

    mNewPathShader.bind();
    mNewPathShader.setUniform2f("frameSeed", seed);
    mNewPathShader.setUniform2f("viewPortSize", glm::vec2(mWidth, mHeight));

    glDispatchCompute(mWidth, mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void SingleBounceRenderer::clearTarget(Texture *target) {
    target->bindAsImage(TARGET_BUFFER_TARGET);
    mClearTargetShader.bind();

    glDispatchCompute(mWidth, mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
