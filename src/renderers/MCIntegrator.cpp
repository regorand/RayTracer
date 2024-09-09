#include "MCIntegrator.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "deviceShared/BufferBindings.h"
#include "imgui.h"

MCIntegrator::MCIntegrator()
    : mcIntegrator(makeComputeProgram("res/shaders/integrators/MonteCarlo.comp"))
{ }

void MCIntegrator::renderSample() {
    if (currentlyWorking()) return;
    if (mCurrentTarget == nullptr) return;

    mcIntegrator.bind();
    mCurrentTarget->bind(TARGET_TEXTURE_TARGET);

    bindUniforms();
    bindScene();

    GLClearError();
    glDispatchCompute(mCurrentTarget->getWidth(), mCurrentTarget->getHeight(), 1);
    GLPrintError();

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
    }
}

void MCIntegrator::bindScene() {
    GPUSceneBuffers &gpuBuffers = mScene->getGPUBuffers();
    gpuBuffers.triangleBuffer.bindToTarget(TRIANGLE_BUFFER_BINDING);
    gpuBuffers.meshBuffer.bindToTarget(MESH_BUFFER_BINDING);
    gpuBuffers.materialBuffer.bindToTarget(MATERIAL_BUFFER_BINDING);
    gpuBuffers.emissiveBuffer.bindToTarget(EMISSIVE_BUFFER_BINDING);
    gpuBuffers.transformationBuffer.bindToTarget(TRANSFORM_BUFFER_BINDING);
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
    mcIntegrator.setUniform1ui("objectCount", mScene->getCPUBuffers().meshes.size());
    mcIntegrator.setUniform1ui("emissiveCount", mScene->getCPUBuffers().emissives.size());
    mcIntegrator.setUniform1f("alphaRR", 0.8);

    mcIntegrator.setUniform1i("width",mCurrentTarget->getWidth());
    mcIntegrator.setUniform1i("height", mCurrentTarget->getHeight());

    mcIntegrator.setUniform1ui("seed", dist.sampleUint());
    Camera &camera = mScene->getActiveCamera();

    mcIntegrator.setUniform3f("cameraPos", camera.getLocation());
    mcIntegrator.setUniform3f("viewDir", camera.getViewDir());
    mcIntegrator.setUniform3f("upDir", camera.getLocalUpVec());

    mcIntegrator.setUniform1f("fov", camera.getIntrinsics().fovY);
    mcIntegrator.setUniform1f("aspect", camera.getIntrinsics().aspectRatio);

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
