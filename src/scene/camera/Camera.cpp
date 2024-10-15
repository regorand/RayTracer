#include <vector>
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera()
    : mName("Camera"), mPosition(0, 0, 3), mViewDir(0, 0, -1), mCameraSpaceUp(UP_ALIGNMENT)
{
    mIntrinsics.aspectRatio = 16.0f / 9.0f;
    mIntrinsics.fovY = glm::pi<float>() / 3;

    mIntrinsics.zNear = 0.1f;
    mIntrinsics.zFar = 1000.0f;
    calculateLocalUp();
}

Camera::Camera(const std::string &name, glm::vec3 position, glm::vec3 viewDir, glm::vec3 up) :
        mName(name), mPosition(position), mViewDir(glm::normalize(viewDir)), mCameraSpaceUp(up) {

    mIntrinsics.aspectRatio = 16.0f / 9.0f;
    mIntrinsics.fovY = glm::pi<float>() / 3;

    mIntrinsics.zNear = 0.1f;
    mIntrinsics.zFar = 1000.0f;
    calculateLocalUp();
}

glm::mat4 Camera::getTransformation() {
    return glm::lookAt(mPosition, mPosition + mViewDir, mLocalUp);
}

glm::vec3 Camera::getCamSpaceUpVec() {
    return mCameraSpaceUp;
}

glm::vec3 &Camera::getViewDir() {
    return mViewDir;
}

void Camera::setViewDir(glm::vec3 viewDir) {
    mViewDir = viewDir;
    calculateLocalUp();
}

void Camera::setLocation(glm::vec3 location) {
    mPosition = location;
}

glm::vec3 &Camera::getLocation() {
    return mPosition;
}

void Camera::buildCameraVisBuffers() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 tangentVec = glm::cross(mLocalUp, mViewDir);
    float upDist = tan(mIntrinsics.fovY / 2);
    float sideDist = upDist * mIntrinsics.aspectRatio;

    vertices.emplace_back(mPosition);
    vertices.emplace_back(mPosition + mViewDir + sideDist * tangentVec + upDist * mLocalUp);
    vertices.emplace_back(mPosition + mViewDir + sideDist * tangentVec - upDist * mLocalUp);
    vertices.emplace_back(mPosition + mViewDir - sideDist * tangentVec - upDist * mLocalUp);
    vertices.emplace_back(mPosition + mViewDir - sideDist * tangentVec + upDist * mLocalUp);

    for (int i = 1; i < 5; i++) {
        indices.push_back(0);
        indices.push_back(i);
    }

    for (int i = 1; i < 4; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indices.push_back(4);
    indices.push_back(1);

    visBuffers.vertexArray.bind();

    visBuffers.arrayBuffer = ArrayBuffer(vertices.data(), sizeof(glm::vec3) * vertices.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // pos
    glEnableVertexAttribArray(0);

    visBuffers.indexBuffer = IndexBuffer(indices.data(), indices.size());
    hasBuffers = true;
}

glm::mat4 Camera::getProjection() {
    return glm::perspective(getIntrinsics().fovY, getIntrinsics().aspectRatio, getIntrinsics().zNear, getIntrinsics().zFar);
}

void Camera::setAspect(float aspect) {
    mIntrinsics.aspectRatio = aspect;
}

void Camera::applyTransform(glm::mat4 transform) {
    transformPosition(transform);
    transformViewDir(transform);
}

void Camera::transformPosition(glm::mat4 transform) {
    glm::vec4 posH = transform * glm::vec4(mPosition, 1);
    mPosition = glm::vec3(posH) / posH.w;

}

void Camera::transformViewDir(glm::mat4 transform) {
    mViewDir = glm::vec3(transform * glm::vec4(mViewDir, 0));

    calculateLocalUp();
}

void Camera::transformCamSpaceUpDir(glm::mat4 transform) {
    mCameraSpaceUp = glm::vec3(transform * glm::vec4(mCameraSpaceUp, 0));
}

glm::vec3 Camera::getLocalUpVec() {
    return mLocalUp;
}

void Camera::calculateLocalUp() {
    glm::vec3 tangent = glm::cross(mViewDir, mCameraSpaceUp);
    mLocalUp = glm::normalize(glm::cross(tangent, mViewDir));
}

void Camera::setName(std::string name) {
    mName = name;
}

std::string Camera::getName() {
    return mName;
}

void Camera::setIntrinsics(const Intrinsics intrinsics) {
    mIntrinsics = intrinsics;
}

bool Camera::hasCameraVisBuffers() {
    return hasBuffers;
}

CameraVisBuffers &Camera::getCameraVisBuffers() {
    return visBuffers;
}

void Camera::copyValues(Camera &other) {
    this->mPosition = other.mPosition;
    this->mViewDir = other.mViewDir;
    this->mIntrinsics = other.mIntrinsics;

    calculateLocalUp();
}
