#include "MCIntegrator.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "deviceShared/BufferBindings.h"
#include "imgui.h"

MCIntegrator::MCIntegrator()
    : mcIntegrator(makeComputeProgram("res/shaders/integrators/MonteCarlo.comp"))
{
    setupQuery();
}

void MCIntegrator::renderSample() {
    // TODO can maybe already queue one more sample here ?
    if (currentlyWorking()) return;
    if (mCurrentTarget == nullptr) return;

    mcIntegrator.bind();
    mCurrentTarget->bind(TARGET_TEXTURE_TARGET);

    bindUniforms();
    bindScene();

    int numWorkGroupsX = mCurrentTarget->getWidth() / WORK_GROUP_SIZE_1_DIM + 1;
    int numWorkGroupsY = mCurrentTarget->getHeight() / WORK_GROUP_SIZE_1_DIM + 1;

    startQuery();

    glDispatchCompute(numWorkGroupsX, numWorkGroupsY, 1);

    endQuery();
    stats.sampleCount++;

    mcIntegrator.unbind();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);


    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    // unbindScene();
}

void MCIntegrator::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Monte Carlo Integrator")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        ImGui::Checkbox("brdf sampling", &doBrdf);
        ImGui::Checkbox("NEE sampling", &doNee);

        if (ImGui::Button("Reload compute shaders", buttonSize)) {
            reloadShaders();
        }

        ImGui::Checkbox("Show statistics", &showStats);

        if (ImGui::CollapsingHeader("More##mcIntegrator_more")) {
            if (ImGui::Checkbox("disable stats", &disableStats)) {
                stats.resetTimings();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("might slightly increase performance");
            }

            ImGui::SliderInt("max bounces", &maxBounces, 0, 16);

            ImGui::Checkbox("do RR", &doRR);
            if (doRR) {
                ImGui::SliderFloat("alpha rr", &alphaRR, 0.0f, 0.99f);
            }
        }
    }

    if (showStats) {

        ImGui::Begin("Render Statistics", &showStats,  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                                          | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("spp: %d", stats.sampleCount);
        ImGui::Text("last: %.3f ms", stats.lastMsTime);
        ImGui::Text("FPS: %.3f",  1000.0 / stats.lastMsTime);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Might not be actual fps, as currently rendering speed tied to VSync if enabled");
        }
        ImGui::Text("glob. avg: %.3f ms", stats.summedTime / stats.timingSamplesCollected);
        ImGui::Text("mov. avg: %.3f ms \n\t(alpha = %.2f)", stats.movingAverage, stats.alpha);
        ImGui::End();
    }
}

void MCIntegrator::bindScene() {
    GPUSceneBuffers &gpuBuffers = mScene->getGPUBuffers();
    gpuBuffers.triangleBuffer.bindToTarget(TRIANGLE_BUFFER_BINDING);
    gpuBuffers.objectBuffer.bindToTarget(OBJECT_BUFFER_BINDING);
    gpuBuffers.meshBuffer.bindToTarget(MESH_BUFFER_BINDING);
    gpuBuffers.materialBuffer.bindToTarget(MATERIAL_BUFFER_BINDING);
    gpuBuffers.emissiveBuffer.bindToTarget(EMISSIVE_BUFFER_BINDING);
    gpuBuffers.transformationBuffer.bindToTarget(TRANSFORM_BUFFER_BINDING);

    GLClearError();
    GLPrintError();
}

void MCIntegrator::unbindScene() {
    GPUSceneBuffers &gpuBuffers = mScene->getGPUBuffers();
    gpuBuffers.triangleBuffer.unbind();
    gpuBuffers.meshBuffer.unbind();
    gpuBuffers.materialBuffer.unbind();
    gpuBuffers.emissiveBuffer.unbind();
    gpuBuffers.transformationBuffer.unbind();
}

bool MCIntegrator::currentlyWorking() {
    if (fence == nullptr) {
        return false;
    }

    GLint signal = 0;
    glGetSynciv(fence, GL_SYNC_STATUS, 1, nullptr, &signal);

    if (signal == GL_SIGNALED) {
        glDeleteSync(fence);
        fence = nullptr;
        return false;
    }

    return true;
}

void MCIntegrator::bindUniforms() {
    mcIntegrator.setUniform1ui("objectCount", mScene->getCPUBuffers().objects.size());
    mcIntegrator.setUniform1ui("emissiveCount", mScene->getCPUBuffers().emissives.size());

    mcIntegrator.setUniform1i("maxBounces", maxBounces);

    mcIntegrator.setUniform1i("doRR", doRR);
    mcIntegrator.setUniform1f("alphaRR", alphaRR);


    int width = mCurrentTarget->getWidth();
    int height = mCurrentTarget->getHeight();

    mcIntegrator.setUniform1i("width", width);
    mcIntegrator.setUniform1i("height", height);

    mcIntegrator.setUniform1ui("seed", dist.sampleUint());
    Camera &camera = mScene->getActiveCamera();

    mcIntegrator.setUniform3f("cameraPos", camera.getLocation());
    mcIntegrator.setUniform3f("viewDir", camera.getViewDir());
    mcIntegrator.setUniform3f("upDir", camera.getLocalUpVec());

    mcIntegrator.setUniform1f("fov", camera.getIntrinsics().fovY);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    mcIntegrator.setUniform1f("aspect", aspect);

    mcIntegrator.setUniform1i("doBrdf", doBrdf ? 1 : 0);
    mcIntegrator.setUniform1i("doNee", doNee ? 1 : 0);
}

void MCIntegrator::setScene(std::shared_ptr<Scene> newScene) {
    mScene = newScene;
}

std::shared_ptr<Scene> MCIntegrator::getScene() {
    return mScene;
}

void MCIntegrator::setTarget(Texture *target) {
    mCurrentTarget = target;
}

Texture *MCIntegrator::getTarget() {
    return mCurrentTarget;
}

void MCIntegrator::reloadShaders() {
    mcIntegrator.reloadShader();
}

RenderStatistics MCIntegrator::getRenderStatistics() {
    return stats;
}

void MCIntegrator::resetRenderStatistics() {
    stats.reset();
}

void MCIntegrator::setupQuery() {
    glGenQueries(1, &timerQuery);
}

void MCIntegrator::handleQueryResult() {
    if (!queryResultPending || disableStats) return;
    GLint64 result;
    glGetQueryObjecti64v(timerQuery, GL_QUERY_RESULT, &result);
    double msResult = (double) result / 1e6;
    stats.lastMsTime = msResult;
    stats.summedTime += msResult;
    stats.movingAverage = stats.alpha * msResult + (1 - stats.alpha) * stats.movingAverage;
    stats.timingSamplesCollected++;
    queryResultPending = false;
}

void MCIntegrator::startQuery() {
    if (disableStats) return;
    glFlush();
    glBeginQuery(GL_TIME_ELAPSED, timerQuery);
}

void MCIntegrator::endQuery() {
    if (disableStats) return;
    glFlush();
    glEndQuery(GL_TIME_ELAPSED);
    queryResultPending = true;
}

bool MCIntegrator::queryAvailable() {
    if (disableStats) return false;
    GLint64 result;
    glGetQueryObjecti64v(timerQuery, GL_QUERY_RESULT_AVAILABLE, &result);
    return result == GL_TRUE;
}
