
#include "Scene.h"
#include "deviceShared/BufferBindings.h"

Scene::Scene():
        mCamera(Camera(glm::vec3(0), glm::vec3(0, 0, -1))) {
}

void Scene::addTriangle(Triangle triangle) {
    mTriangles.push_back(triangle);
}

void Scene::setCamera(Camera camera) {
    mCamera = camera;
}

void Scene::createSSBO() {
    // TODO
    // 1. rewrite Scene to accept Objects -> Object class (Only Mesh objects or also analytical ones ?)
    // 2. accept Materials
    // 3. Ability to make Objects Emissive by index -> nothing happens if invalid index ?

    mSSBO = SSBO(mTriangles.data(), sizeof(Triangle) * mTriangles.size(), GL_STATIC_READ, TRIANGLE_BUFFER_BINDING);
}

SSBO &Scene::getSSBO() {
    return mSSBO;
}
