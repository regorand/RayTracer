
#include "Scene.h"

Scene::Scene():
        mCamera(Camera(glm::vec3(0), glm::vec3(0, 0, -1))) {
    mSpheres.push_back({glm::vec3(0, 0, -10), 3, glm::vec3(1, 1, 0)});
    mSpheres.push_back({glm::vec3(15, 0, -25), 7, glm::vec3(0, 1, 1)});
}

std::vector<Sphere> &Scene::getSpheres() {
    return mSpheres;
}

Camera &Scene::getCamera() {
    return mCamera;
}