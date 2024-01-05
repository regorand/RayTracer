
#include <iostream>
#include "Scene.h"
#include "deviceShared/BufferBindings.h"
#include <glm/gtc/matrix_inverse.hpp>

Scene::Scene():
        mCamera(Camera(glm::vec3(0), glm::vec3(0, 0, -1))) {
}

void Scene::setCamera(Camera camera) {
    mCamera = camera;
}

Camera &Scene::getCamera() {
    return mCamera;
}

void Scene::createGPUBuffers() {
    std::vector<Triangle> triangles;
    std::vector<BufferMeshObject> objects;
    std::vector<Material> materials;
    std::vector<unsigned int> emissiveIndices;

    unsigned int triangleOffset = 0;
    unsigned int objectOffset = 0;

    // TODO store and then be able to return object count and emissive count here!
    for (auto &object: mObjects) {
        auto &objectTriangles = object.getTriangleBuffer();
        unsigned int triangleCount = objectTriangles.size();

        BufferMeshObject bmo;
        bmo.invTransform = glm::inverse(object.getTransform().getMatrix());
        bmo.normalMat = glm::transpose(bmo.invTransform);
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

void Scene::addMeshObject(MeshObject &meshObject) {
    mObjects.push_back(meshObject);
}

void Scene::bind() {
    if (!sceneBuffers.isValid()) {
        std::cerr << "Scene can not be bound, scene buffers are not valid" << std::endl;
        return;
    }

    sceneBuffers.bind();
}

void Scene::unbind() {
    if (!sceneBuffers.isValid()) {
        std::cerr << "Scene can not be unbound, scene buffers are not valid" << std::endl;
        return;
    }

    sceneBuffers.unbind();
}

std::vector<MeshObject> &Scene::getObjects() {
    return mObjects;
}

void Scene::buildRasterizeBuffers() {
    for (auto &object: mObjects) {
        object.createRasterBuffers();
    }
}

bool Scene::canRasterize() {
    for (auto &object: mObjects) {
        if (!object.canRasterize()) return false;
    }
    for (auto &path: mPaths) {
        if (!path.canRasterize()) return false;
    }
    return true;
}

void Scene::addPathObject(PathVisObject &pathVisObject) {
    pathVisObject.createRasterBuffers();
    mPaths.push_back(pathVisObject);
}

std::vector<PathVisObject> &Scene::getPathObjects() {
    return mPaths;
}

void Scene::resetPaths() {
    mPaths.clear();
}
