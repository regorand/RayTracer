#include "MCIntegrator.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "imgui.h"

MCIntegrator::MCIntegrator() {
    ShaderProgram mcIntegrator = makeComputeProgram("res/shaders/integrators/MonteCarlo.comp");
    mcIntegrator.setName("Pathtracer");
    shaders.push_back(mcIntegrator);

    ShaderProgram normal = makeComputeProgram("res/shaders/misc/displayNormals.comp");
    normal.setName("Normals");
    shaders.push_back(normal);
}

void MCIntegrator::renderSample() {
    // TODO can maybe already queue one more sample here ?
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

void MCIntegrator::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Monte Carlo Integrator")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        ImGui::Checkbox("brdf sampling", &doBrdf);
        ImGui::Checkbox("NEE sampling", &doNee);

        if (ImGui::Button("Reload compute shaders", buttonSize)) {
            reloadShaders();
        }

        ImGui::Checkbox("Show statistics##MCIntegrator", &showStats);

        if (ImGui::CollapsingHeader("More##mcIntegrator_more")) {
            ImGui::Text("Camera Manipulator");
            if (ImGui::BeginCombo("##chooseShader_mcIntegrator", shaders[activeShader].getName().c_str())) {
                for (int i = 0; i < shaders.size(); i++) {
                    bool is_selected = activeShader == i; // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(shaders[i].getName().c_str(), is_selected)) {
                        activeShader = i;
                        dirty = true;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)

                }

                ImGui::EndCombo();
            }


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

void MCIntegrator::bindUniforms() {
    ShaderProgram &mcIntegrator = shaders[activeShader];
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
