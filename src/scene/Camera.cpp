
#include <vector>
#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 viewDir):
    mPosition(position), mViewDir(glm::normalize(viewDir)) {

    mIntrinsics.aspectRatio = 16.0f / 9.0f;
    mIntrinsics.yfov = glm::pi<float>() / 3;
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

void Camera::buildCameraVisBuffers(CameraVisBuffers &buffers) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 tangentVec = glm::cross(UP_VECTOR, mViewDir);
    float upDist = tan(mIntrinsics.yfov / 2);
    float sideDist = upDist * mIntrinsics.aspectRatio;

    vertices.emplace_back(mPosition);
    vertices.emplace_back(mPosition + mViewDir + sideDist * tangentVec + upDist * UP_VECTOR);
    vertices.emplace_back(mPosition + mViewDir + sideDist * tangentVec - upDist * UP_VECTOR);
    vertices.emplace_back(mPosition + mViewDir - sideDist * tangentVec - upDist * UP_VECTOR);
    vertices.emplace_back(mPosition + mViewDir - sideDist * tangentVec + upDist * UP_VECTOR);

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

    buffers.vertexArray.bind();

    buffers.arrayBuffer = ArrayBuffer(vertices.data(), sizeof(glm::vec3) * vertices.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); // pos
    glEnableVertexAttribArray(0);

    buffers.indexBuffer = IndexBuffer(indices.data(), indices.size());
}

glm::mat4 Camera::getProjection() {
    return glm::perspective(getIntrinsics().yfov, getIntrinsics().aspectRatio, 0.1f, 1000.0f);
}

void Camera::setAspect(float aspect) {
    mIntrinsics.aspectRatio = aspect;
}
