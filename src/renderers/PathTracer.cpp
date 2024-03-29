
#include <iostream>
#include <array>
#include "PathTracer.h"
#include "imgui.h"
#include "deviceShared/BufferBindings.h"
#include "dataStructures/SettingIndex.h"

PathTracer::PathTracer(int width, int height)
        : mWidth(width),
          mHeight(height),

          buffers(mWidth * mHeight * sizeof(float) * 4, GL_DYNAMIC_DRAW),

          mBounceShader(ComputeShader("res/shaders/compute/computeSingleBounce.comp")),
          mNewPathShader(ComputeShader("res/shaders/compute/CreateNewPath.comp")),
          mClearTargetShader(ComputeShader("res/shaders/compute/clearColorBuffer.comp")) {
}

void PathTracer::startRender() {
    if (mCurrentTarget != nullptr) {
        mRenderStatus.state = RenderState::DUE_NEW_PATH;
        mRenderStatus.targetSpp = mPathTraceSettings.spp;
        mRenderStatus.currentSpp = 0;

        renderStart = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
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

            milliseconds renderEnd = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            );

            milliseconds renderTime = renderEnd - renderStart;
            std::cout << "Render took "
                        << renderTime.count()
                        << " ms for "
                        << status.currentSpp
                        << " samples.\n"
                        << renderTime.count() / status.currentSpp
                        << " ms per sample."
                        << std::endl;

            return RenderState::IDLE;
        } else {
            status.state = RenderState::DUE_NEW_PATH;
        }
    }

    if (status.state == RenderState::DUE_NEW_PATH) {
        newPath();

        /*
         * Required for denoiser
         * 1. pos
         * 2. normal
         * 3. albedo
         *
         * Required for bounce
         * 1.
         *
         * TODO
         * 1. rasterize, store values for denoiser in buffers
         * 2. call compute shader to use denoiser buffer values to create bounce values, store in pathtracer buffers
         * 3. call bounces as usual
         */
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
    // glm::vec2 seed = dist.sampleVec2();
    float seed = dist.sampleFloat();

    if (enableRecording && !recordedPath.vertices.empty()) {
        recordedPath.vertices.erase(recordedPath.vertices.begin(), recordedPath.vertices.end());
        recordedPath.pathLength = 0;
    }

    mCurrentTarget->bindAsImage(TARGET_TEXTURE_TARGET);

    buffers.posBuffer.bindToTarget(POS_BUFFER_TARGET);
    buffers.dirBuffer.bindToTarget(DIR_BUFFER_TARGET);
    buffers.multiplierBuffer.bindToTarget(MULT_BUFFER_TARGET);

    mNewPathShader.bind();
    mNewPathShader.setUniform1f("frameSeed", seed);
    mNewPathShader.setUniform2f("viewPortSize", glm::vec2(mWidth, mHeight));

    mNewPathShader.setUniform1i("width", mWidth);

    Camera &camera = mScene.getCamera();

    mNewPathShader.setUniform3f("cameraPos", camera.getLocation());
    mNewPathShader.setUniform3f("viewDir", camera.getViewDir());
    mNewPathShader.setUniform3f("upDir", camera.getLocalUpVec());

    mNewPathShader.setUniform1f("fov", camera.getIntrinsics().fovY);
    mNewPathShader.setUniform1f("aspect", camera.getIntrinsics().aspectRatio);

    glDispatchCompute(mWidth, mHeight, 1);

    mNewPathShader.unbind();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    recordPathVertex();
}

void PathTracer::renderSingleBounce() {

    glm::vec2 seed = dist.sampleVec2();

    // mBounceShader.setUniform1f("seed", seed.x);
    mBounceShader.setUniform1ui("seed", dist.sampleUint());
    mBounceShader.setUniform2f("frameSeed", seed);
    glDispatchCompute(mWidth, mHeight, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    recordPathVertex();
}

void PathTracer::clearTarget() {
    // TODO extract to TextureUtils or smth

    mSppCount = 0;
    mCurrentTarget->bindAsImage(TARGET_TEXTURE_TARGET);
    mClearTargetShader.bind();

    glDispatchCompute(mWidth, mHeight, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

void PathTracer::setPathTraceSettings(PathTraceSettings pathTraceSettings) {
    mPathTraceSettings = pathTraceSettings;
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
    mCurrentTarget->bindAsImage(TARGET_TEXTURE_TARGET);

    buffers.posBuffer.bindToTarget(POS_BUFFER_TARGET);
    buffers.dirBuffer.bindToTarget(DIR_BUFFER_TARGET);
    buffers.multiplierBuffer.bindToTarget(MULT_BUFFER_TARGET);

    mScene.bind();

    // TODO get scene counts and bind as uniforms, then add stuff to the scene, try to only render albedo first

    mBounceShader.bind();

    mBounceShader.setUniform1ui("objectCount", mScene.getObjectCount());
    mBounceShader.setUniform1ui("emissiveCount", mScene.getEmissiveCount());
    mBounceShader.setUniform1f("alphaRR", mPathTraceSettings.pathSettings.alphaRR);

    mBounceShader.setUniform1i("width", mWidth);
    // mBounceShader.setUniform1i("height", mHeight);
}

void PathTracer::unbindBounce() {
    mBounceShader.unbind();

    mScene.unbind();
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

void PathTracer::setScene(OldScene &scene) {
    mScene = scene;
}

void PathTracer::buildImGuiWindow(DataStore &settings) {
    PathSettings &pathSettings = mPathTraceSettings.pathSettings;
    if (ImGui::CollapsingHeader("Path Tracer Control")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        ImGui::Checkbox("Show Pathtrace debug",
                        settings.getPointer<bool>(SHOW_PATHTRACE_DEBUG)
        );

        if (settings.getOrElse(SHOW_PATHTRACE_DEBUG, false)) {
            auto items = PathTracerBuffers::getAsList();
            int currentIndex = settings.getOrInit<int>(PATHTRACE_DEBUG_BUFFER_INDEX);
            currentIndex = glm::min(glm::max(currentIndex, 0), (int) items.size() - 1);

            std::string selected = items[currentIndex];
            if (ImGui::BeginCombo(" Buffer", items[currentIndex].c_str())) {
                for (int i = 0; i < items.size(); i++) {
                    bool is_selected = (selected == items[i]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(items[i].c_str(), is_selected)) {
                        selected = items[i];
                        currentIndex = i;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)

                }

                ImGui::EndCombo();
                settings.store(PATHTRACE_DEBUG_BUFFER_INDEX, currentIndex);
            }

        }

        if (ImGui::Button("Reload compute shaders", buttonSize)) {
            reloadShaders();
        }

        bool currentlyRendering = mRenderStatus.state != RenderState::IDLE;

        if (!currentlyRendering && ImGui::Button("Render", buttonSize)) {
            startRender();
        }

        if (currentlyRendering && ImGui::Button("Cancel Render", buttonSize)) {
            cancelRender();
        }

        if (ImGui::Button("Clear Color Buffer", buttonSize)) {
            clearTarget();
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

void PathTracer::enablePathRecording(bool enable) {
    enableRecording = enable;
}

bool PathTracer::hasRecordedPath() {
    return !recordedPath.vertices.empty();
}

Path &PathTracer::getRecordedPath() {
    return recordedPath;
}

void PathTracer::setRecordingCoords(glm::vec2 coords) {
    recordingCoords = coords;
}

void PathTracer::recordPathVertex() {
    if (enableRecording) {
        GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

        GLuint64 timeoutNs = 10ll * 1000l * 1000l * 1000l;
        glClientWaitSync(sync, 0, timeoutNs);

        GLint signal = 0;
        glGetSynciv(sync, GL_SYNC_STATUS, 1, nullptr, &signal);

        while (signal != GL_SIGNALED) {
            glGetSynciv(sync, GL_SYNC_STATUS, 1, nullptr, &signal);
        }

        int offsetIndex = recordingCoords.y * mWidth + recordingCoords.x;
        int offset = offsetIndex * sizeof(glm::vec4);

        glm::vec4 vertex;
        buffers.posBuffer.getData(&vertex, sizeof(glm::vec4), offset);

        recordedPath.vertices.emplace_back(vertex);
        recordedPath.pathLength++;
        glDeleteSync(sync);
    }
}

OldScene &PathTracer::getSceneRef() {
    return mScene;
}
PathTracerBuffers &PathTracer::getBuffers() {
    return buffers;
}

void PathTracer::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    buildImGuiWindow(settings);
}
