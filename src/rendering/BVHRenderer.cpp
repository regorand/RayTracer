#include "BVHRenderer.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "imgui.h"

BVHRenderer::BVHRenderer() {
    ShaderProgram mcIntegrator = makeComputeProgram("res/shaders/misc/visualizeBVH.comp");
    mcIntegrator.setName("Pathtracer");
    shaders.push_back(mcIntegrator);
}

void BVHRenderer::renderSample() {
    if (currentlyWorking()) return;
    if (mCurrentTarget == nullptr) return;

    shaders[activeShader].bind();
    mCurrentTarget->bind(TARGET_TEXTURE_TARGET);

    bindUniforms();
    bindScene();

    int numWorkGroupsX = mCurrentTarget->getWidth() / WORK_GROUP_SIZE_1_DIM + 1;
    int numWorkGroupsY = mCurrentTarget->getHeight() / WORK_GROUP_SIZE_1_DIM + 1;

    startQuery();

    glDispatchCompute(numWorkGroupsX, numWorkGroupsY, 1);

    endQuery();
    stats.sampleCount++;

    shaders[activeShader].unbind();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    // unbindScene();

}

void BVHRenderer::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("BVH Integrator")) {
        if (ImGui::Button("reload shaders")) {
            reloadShaders();
        }
        ImGui::Checkbox("Show statistics##BVHRenderer", &showStats);

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

void BVHRenderer::bindUniforms() {
    ShaderProgram &shader = shaders[activeShader];

    shader.setUniform1ui("objectCount", mScene->getCPUBuffers().objects.size());


    int width = mCurrentTarget->getWidth();
    int height = mCurrentTarget->getHeight();

    shader.setUniform1i("width", width);
    shader.setUniform1i("height", height);

    shader.setUniform1ui("seed", dist.sampleUint());
    Camera &camera = mScene->getActiveCamera();

    shader.setUniform3f("cameraPos", camera.getLocation());
    shader.setUniform3f("viewDir", camera.getViewDir());
    shader.setUniform3f("upDir", camera.getLocalUpVec());

    shader.setUniform1f("fov", camera.getIntrinsics().fovY);

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    shader.setUniform1f("aspect", aspect);

}
