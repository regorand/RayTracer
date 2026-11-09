#include <memory>
#include <utility>
#include "Rasterizer.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glWrappers/Texture.h"
#include "deviceShared/BufferBindings.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "imgui.h"
#include "gui/ImguiUtils.h"
#include "glm/ext/matrix_clip_space.hpp"

Rasterizer::Rasterizer()
        : mShader(makeRasterizeProgram("res/shaders/raster/test_new.vert", "res/shaders/raster/test_new.frag")),
          mGeometryPhaseShader(makeRasterizeProgram("res/shaders/raster/simpleScene.vert",
                                                    "res/shaders/raster/saveToBuffers.frag")),
          mCameraVisShader(makeRasterizeProgram("res/shaders/raster/rasterizeCamFrustum.vert", "res/shaders/raster/rasterizeCamFrustum.frag"))
          { }

void Rasterizer::render(int width, int height) {
    if (!canRasterize()) return;
    Camera &camera = mScene->getActiveCamera();

    glm::mat4 cameraMat = camera.getTransformation();

    const auto &intrinsics = camera.getIntrinsics();

    float aspect = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projectionMat = glm::perspective(intrinsics.fovY, aspect, intrinsics.zNear, intrinsics.zFar);

    glm::mat4 VP = projectionMat * cameraMat;

    glEnable(GL_DEPTH_TEST);

    mScene->bindRaster();
    auto &transforms = mScene->getCPUBuffers().transforms;
    mScene->getMatBuffer().bindToTarget(3);

    mShader.bind();

    auto &meshes = mScene->getMeshes();

    // for (auto &mesh: mScene->getMeshes()) {
    for (auto &object: mScene->getObjects()) {
        auto &mesh = meshes[object.meshID];

        glm::mat4 model = transforms[object.transformID];
        glm::mat4 MVP = VP * model;
        glm::mat4 normalTransform = glm::inverseTranspose(model);

        // mShader.setUniformMat4f("transform", model);
        mShader.setUniformMat4f("MVP", MVP);
        mShader.setUniformMat4f("normalTransform", normalTransform);


        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        int indexBufferIndex = mesh.indexOffset;
        int indexCount = mesh.indexCount;

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *) (indexBufferIndex * sizeof(GLuint)));
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    mShader.unbind();
    mScene->getMatBuffer().unbind();

    mScene->unbindRaster();

    if (renderCamFrustums) {
        auto &staticCams = mScene->getStaticCameras();
        for (int i = 0; i < staticCams.size(); i++) {
            Camera &visCam = staticCams[i];
            bool hovered = i == mScene->getHoveredCameraIndex();
            renderCameraFrustum(visCam, hovered);
        }
    }
}

void Rasterizer::setScene(std::shared_ptr<Scene> newScene) {
    mScene = std::move(newScene);
}

bool Rasterizer::canRasterize() {
    return mScene != nullptr;
}

void Rasterizer::renderCameraFrustum(Camera &camera, bool isHovered) {
    CameraVisBuffers &buffers = camera.getCameraVisBuffers();

    Camera activeCamera = mScene->getActiveCamera();
    glm::mat4 cameraMat = activeCamera.getTransformation();
    glm::mat4 projectionMat = activeCamera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;

    mCameraVisShader.bind();
    buffers.vertexArray.bind();
    buffers.indexBuffer.bind();

    mCameraVisShader.setUniformMat4f("VP", VP);
    mCameraVisShader.setUniform3f("displayColor", isHovered ? glm::vec3(1.0f, 0.5f, 0.2f) : glm::vec3(1.0f));

    glDrawElements(GL_LINES, buffers.indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr);
    mCameraVisShader.unbind();
}

std::shared_ptr<Scene> Rasterizer::getScene() {
    return mScene;
}

void Rasterizer::reloadShaders() {
    mShader.reloadShader();
    mGeometryPhaseShader.reloadShader();
    mCameraVisShader.reloadShader();
}

void Rasterizer::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Rasterizer")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        startIndent();
        if (ImGui::Button("Reload shaders", buttonSize)) {
            reloadShaders();
        }
        ImGui::Checkbox("Render Camera Frustums", &renderCamFrustums);
        endIndent();
    }
}

