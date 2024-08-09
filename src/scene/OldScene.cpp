
#include <iostream>
#include "OldScene.h"
#include "deviceShared/BufferBindings.h"
#include "imgui.h"
#include "gui/ImguiUtils.h"
#include <glm/gtc/matrix_inverse.hpp>

OldScene::OldScene():
        mCamera(Camera(glm::vec3(0), glm::vec3(0, 0, -1))) {
}

void OldScene::setCamera(Camera camera) {
    mCamera = camera;
}

Camera &OldScene::getCamera() {
    return mCamera;
}

void OldScene::createGPUBuffers() {
    std::vector<Triangle> triangles;
    std::vector<BufferMeshObject> objects;
    std::vector<MaterialOld> materials;
    std::vector<unsigned int> emissiveIndices;

    unsigned int triangleOffset = 0;
    unsigned int objectOffset = 0;

    // TODO store and then be able to return object count and emissive count here!
    for (auto &object: mObjects) {
        auto &objectTriangles = object.getTriangleBuffer();
        unsigned int triangleCount = objectTriangles.size();

        BufferMeshObject bmo;
        // bmo.invTransform = glm::inverse(object.getTransform().getMatrix());
        // bmo.normalMat = glm::transpose(bmo.invTransform);
        bmo.aabb = object.createAABB();
        bmo.emissive = object.isEmissive();
        bmo.firstTriangleIndex = triangleOffset;
        bmo.triangleCount = triangleCount;

        triangleOffset += triangleCount;

        triangles.insert(triangles.end(), objectTriangles.begin(), objectTriangles.end());

        objects.push_back(bmo);

        // TODO materials

        if (bmo.emissive) {
            emissiveIndices.push_back(objectOffset);
        }
        objectOffset++;
    }

    mObjectCount = objects.size();
    mEmissiveCount = emissiveIndices.size();

    sceneBuffers.createGPUBuffers(triangles, objects, materials, emissiveIndices);
}

void OldScene::addMeshObject(MeshObject &meshObject) {
    mObjects.push_back(meshObject);
}

void OldScene::bind() {
    if (!sceneBuffers.isValid()) {
        std::cerr << "Scene can not be bound, scene buffers are not valid" << std::endl;
        return;
    }

    sceneBuffers.bind();
}

void OldScene::unbind() {
    if (!sceneBuffers.isValid()) {
        std::cerr << "Scene can not be unbound, scene buffers are not valid" << std::endl;
        return;
    }

    sceneBuffers.unbind();
}

std::vector<MeshObject> &OldScene::getObjects() {
    return mObjects;
}

void OldScene::buildRasterizeBuffers() {
    for (auto &object: mObjects) {
        object.createRasterBuffers();
    }
}

bool OldScene::canRasterize() {
    for (auto &object: mObjects) {
        if (!object.canRasterize()) return false;
    }
    for (auto &path: mPaths) {
        if (!path.canRasterize()) return false;
    }
    return true;
}

void OldScene::addPathObject(PathVisObject &pathVisObject) {
    pathVisObject.createRasterBuffers();
    mPaths.push_back(pathVisObject);
}

std::vector<PathVisObject> &OldScene::getPathObjects() {
    return mPaths;
}

void OldScene::resetPaths() {
    mPaths.clear();
}

void OldScene::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Scene")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        startIndent();
        buildImguiObjectList();
        endIndent();
    }
}

void OldScene::buildImguiObjectList() {
    if (ImGui::CollapsingHeader("Objects")) {

        startIndent();
        for (size_t i = 0; i < mObjects.size(); i++) {
            buildObjectImgui(mObjects[i], i);
            endAndNewIndent();
        }
        endIndent();

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        ImGui::Text("Scene");
    }
}

void OldScene::buildObjectImgui(MeshObject &object, size_t index) {
    auto label = "Object " + std::to_string(index);
    if (ImGui::CollapsingHeader(label.c_str())) {
        ImGui::Spacing();
        ImGui::Text("Triangle Count: %d", object.getTriangles().size());
        ImGui::Text("Emissive: %s", object.isEmissive() ? "true" : "false");
    }
}
