
#include "Scene.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/buffers/import_scene_error.h"
#include "imgui.h"
#include "gui/ImguiUtils.h"
#include "SceneImporter.h"
#include "scene/camera/Camera.h"

void Scene::addCamera(Camera camera) {
    cameraController.addCamera(camera);
}

bool Scene::loadFile(std::string path) {
    SceneImporter importer(path);
    importer.import(cpuBuffers);

    buildRasterBuffers();

    buildGPUBuffers();

    return true;
}

Camera &Scene::getActiveCamera() {
    return cameraController.getCameraByIndex(activeCameraIndex);
}

void Scene::buildMaterialBuffer() {
    std::vector<BufferMaterial > data;
    data.reserve(cpuBuffers.materials.size());
    for (auto &material: cpuBuffers.materials) {
        data.push_back(material.bufferMaterial);
    }

    gpuBuffers.materialBuffer = { static_cast<unsigned int>(data.size() * sizeof(BufferMaterial)), GL_STATIC_DRAW, data.data() };
}

Scene::~Scene() {
    gpuBuffers.materialBuffer.destroy();
}

void Scene::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Scene")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        startIndent();
        ImGui::Text("Scene Stuff");
        endIndent();
    }

}

std::vector<Mesh> &Scene::getMeshes() {
    return cpuBuffers.meshes;
}

void Scene::bindRaster() {
    mRasterBuffersNew.mVa.bind();
    mRasterBuffersNew.mIb.bind();

}

void Scene::unbindRaster() {
    mRasterBuffersNew.mVa.unbind();
    mRasterBuffersNew.mIb.unbind();
}

CPUSceneBuffers &Scene::getCPUBuffers() {
    return cpuBuffers;
}

GPUSceneBuffers &Scene::getGPUBuffers() {
    return gpuBuffers;
}

ShaderStorageBuffer &Scene::getMatBuffer() {
    return gpuBuffers.materialBuffer;
}

void Scene::buildRasterBuffers() {
    mRasterBuffersNew.mVa.bind();

    GLsizei stride = sizeof(RasterVertex);

    mRasterBuffersNew.mAb = ArrayBuffer(cpuBuffers.vertices.data(), sizeof(RasterVertex) * cpuBuffers.vertices.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0); // pos
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) sizeof(glm::vec3)); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *) (2 * sizeof(glm::vec3))); // uv
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, stride, (void *) (2 * sizeof(glm::vec3) + sizeof(glm::vec2))); // material
    glEnableVertexAttribArray(3);

    mRasterBuffersNew.mIb = IndexBuffer(cpuBuffers.indices.data(), cpuBuffers.indices.size());
}

void Scene::buildGPUBuffers() {
    buildMaterialBuffer();

    buildMeshBuffer();


    gpuBuffers.triangleBuffer = { static_cast<unsigned int>(cpuBuffers.triangles.size() * sizeof(Triangle)),
                                  GL_STATIC_DRAW,
                                  cpuBuffers.triangles.data() };


    gpuBuffers.emissiveBuffer = { static_cast<unsigned int>(cpuBuffers.emissives.size() * sizeof(unsigned int)),
                                  GL_STATIC_DRAW,
                                  cpuBuffers.emissives.data() };


    gpuBuffers.transformationBuffer = { static_cast<unsigned int>(cpuBuffers.transforms.size() * sizeof(glm::mat4)),
                                        GL_STATIC_DRAW,
                                        cpuBuffers.transforms.data() };

}

void Scene::buildMeshBuffer() {
    std::vector<BufferMesh > data;
    data.reserve(cpuBuffers.meshes.size());
    for (auto &mesh: cpuBuffers.meshes) {
        data.push_back(mesh.bufferMesh);
    }

    gpuBuffers.meshBuffer = {static_cast<unsigned int>(data.size() * sizeof(BufferMesh)), GL_STATIC_DRAW, data.data() };

}