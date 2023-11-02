
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 viewDir):
    mPosition(position), mViewDir(glm::normalize(viewDir)) {

    mIntrinsics.aspectRatio = 9.0 / 16;
    mIntrinsics.fov = glm::pi<float>() / 2;
}

glm::mat4 Camera::getTransformation() {
    return glm::lookAt(mPosition, mPosition + mViewDir, UP_VECTOR);
}

glm::vec3 Camera::getUpVec() {
    return UP_VECTOR;
}

glm::vec3 &Camera::getViewDir() {
    return mViewDir;
}

void Camera::setViewDir(glm::vec3 viewDir) {
    mViewDir = viewDir;
}

void Camera::setLocation(glm::vec3 location) {
    mPosition = location;
}

glm::vec3 &Camera::getLocation() {
    return mPosition;
}
