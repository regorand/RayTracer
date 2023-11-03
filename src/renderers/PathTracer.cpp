
#include <iostream>
#include "PathTracer.h"
#include "imgui.h"

PathTracer::PathTracer(int width, int height)
        : mWidth(width),
          mHeight(height),
          mPosBuffer(Texture(new HostBuffer(mWidth, mHeight))),
          mDirBuffer(Texture(new HostBuffer(mWidth, mHeight))),
          mMultiplierBuffer(Texture(new HostBuffer(mWidth, mHeight))),

          mBounceShader(ComputeShader("res/shaders/compute/computeSingleBounce.comp")),
          mNewPathShader(ComputeShader("res/shaders/compute/CreateNewPath.comp")),
          mClearTargetShader(ComputeShader("res/shaders/compute/clearColorBuffer.comp")) {
}

void PathTracer::startRender() {
    if (mCurrentTarget != nullptr) {
        mRenderStatus.state = RenderState::DUE_NEW_PATH;
        mRenderStatus.targetSpp = mPathTraceSettings.spp;
        mRenderStatus.currentSpp = 0;
    } else {
        std::cerr << "Can not start render, missing target" << std::endl;
    }
}

RenderState PathTracer::doRenderStep() {
    RenderStatus &status = mRenderStatus;
    if (status.state == RenderState::IDLE) {
        std::cerr << "Need to have started a render before performing a render step" << std::endl;
        return RenderState::IDLE;
    }

    if (status.state == RenderState::BUSY) {
        if (!renderFenceSignalled()) {
            return RenderState::BUSY;
        }
        status.state = RenderState::DUE_BOUNCE;
    }

    if (status.state == RenderState::DUE_BOUNCE
        && mPathTraceSettings.pathSettings.alphaRR < dist.sampleFloat()) {

        mSppCount++;
        status.currentSpp++;
        if (status.currentSpp == status.targetSpp) {
            status.state = RenderState::IDLE;
            return RenderState::IDLE;
        } else {
            status.state = RenderState::DUE_NEW_PATH;
        }
    }

    if (status.state == RenderState::DUE_NEW_PATH) {
        newPath();
    }

    bindBounce();
    renderSingleBounce();
    unbindBounce();

    status.state = RenderState::BUSY;
    mRenderStatus.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    return status.state;
}

void PathTracer::cancelRender() {
    mRenderStatus.reset();
}

void PathTracer::reloadShaders() {
    mBounceShader.reloadShader();
    mNewPathShader.reloadShader();
    mClearTargetShader.reloadShader();
}

void PathTracer::newPath() {
    glm::vec2 seed = dist.sampleVec2();

    mPosBuffer.bindAsImage(POS_BUFFER_TARGET);
    mDirBuffer.bindAsImage(DIR_BUFFER_TARGET);
    mMultiplierBuffer.bindAsImage(MULT_BUFFER_TARGET);

    mNewPathShader.bind();
    mNewPathShader.setUniform2f("frameSeed", seed);
    mNewPathShader.setUniform2f("viewPortSize", glm::vec2(mWidth, mHeight));

    glDispatchCompute(mWidth, mHeight, 1);

    mNewPathShader.unbind();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void PathTracer::renderSingleBounce() {
    glm::vec2 seed = dist.sampleVec2();

    mBounceShader.setUniform2f("frameSeed", seed);
    glDispatchCompute(mWidth, mHeight, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void PathTracer::clearTarget() {
    mSppCount = 0;
    mCurrentTarget->bindAsImage(TARGET_BUFFER_TARGET);
    mClearTargetShader.bind();

    glDispatchCompute(mWidth, mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

void PathTracer::setPathTraceSettings(PathTraceSettings pathTraceSettings) {
    mPathTraceSettings = pathTraceSettings;
}

void PathTracer::buildImGuiWindow() {
    PathSettings &pathSettings = mPathTraceSettings.pathSettings;
    if (ImGui::CollapsingHeader("Path Tracer Control")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reload compute shaders", buttonSize)) {
            reloadShaders();
        }

        if (ImGui::Button("Render", buttonSize)) {
            startRender();
        }

        if (ImGui::Button("Clear Color Buffer", buttonSize)) {
            clearTarget();
        }

        if (mRenderStatus.state != RenderState::IDLE && ImGui::Button("Cancel Render", buttonSize)) {
            cancelRender();
        }

        ImGui::InputInt("Target SPP per render", &mPathTraceSettings.spp);

        std::string sppString = "SPP of current image: " + std::to_string(mSppCount);
        ImGui::Text("%s", sppString.c_str());

        if (ImGui::CollapsingHeader("Path Settings")) {
            ImGui::Indent(8.0f);
            ImGui::Checkbox("Russian Roulette", &pathSettings.russianRoulette);
            if (pathSettings.russianRoulette) {
                ImGui::SliderFloat("Alpha", &pathSettings.alphaRR, 0.1, 0.9);
            } else {
                ImGui::SliderInt("Path length", &pathSettings.constantPathLength, 1, 10);
            }
            ImGui::Unindent(8.0f);
        }
    }
}

RenderStatus PathTracer::getRenderStatus() {
    return mRenderStatus;
}

RenderState PathTracer::getRenderState() {
    return mRenderStatus.state;
}

void PathTracer::setTargetp(Texture *target) {
    this->mCurrentTarget = target;
}

void PathTracer::bindBounce() {
    mCurrentTarget->bindAsImage(TARGET_BUFFER_TARGET);

    mPosBuffer.bindAsImage(POS_BUFFER_TARGET);
    mDirBuffer.bindAsImage(DIR_BUFFER_TARGET);
    mMultiplierBuffer.bindAsImage(MULT_BUFFER_TARGET);

    mBounceShader.bind();

}

void PathTracer::unbindBounce() {
    mBounceShader.unbind();

}

bool PathTracer::renderFenceSignalled() {
    if (mRenderStatus.state == RenderState::IDLE || mRenderStatus.sync == nullptr) {
        return true;
    }

    GLint signal = 0;
    glGetSynciv(mRenderStatus.sync, GL_SYNC_STATUS, 1, nullptr, &signal);

    if (signal == GL_SIGNALED) {
        glDeleteSync(mRenderStatus.sync);
        return true;
    }
    return false;
}

PathTraceSettings PathTracer::getPathTraceSettings() {
    return mPathTraceSettings;
}

void PathTracer::buildImGuiProgressBar(glm::vec2 displaySize) {
    if (mRenderStatus.state == RenderState::IDLE) return;

    ImVec2 targetWindowSize = ImVec2(400, 45);

    int xPos = (static_cast<int>(displaySize.x) - targetWindowSize.x) / 2;
    const int bottomDist = 50;
    int yPos = static_cast<int>(displaySize.y) - bottomDist - targetWindowSize.y;

    ImGui::SetNextWindowSize(targetWindowSize);
    ImGui::SetNextWindowPos(ImVec2(xPos, yPos));

    ImGui::Begin("Render Progress", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                 | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMouseInputs
                 | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    float fraction = static_cast<float>(mRenderStatus.currentSpp) / static_cast<float>(mRenderStatus.targetSpp);

    ImGui::ProgressBar(fraction, ImGui::GetContentRegionAvail());

    ImGui::End();
}
