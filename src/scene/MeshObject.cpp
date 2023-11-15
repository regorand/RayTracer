
#include "MeshObject.h"
#include "glm/ext/matrix_transform.hpp"

void MeshObject::addTriangle(Triangle triangle) {
    mTriangles.push_back(triangle);
}

std::vector<Triangle> &MeshObject::getTriangleBuffer() {
    return mTriangles;
}

AABB MeshObject::createAABB() {
    AABB aabb { glm::vec3(INFINITY), glm::vec3(-INFINITY) };

    for (auto &triangle: mTriangles) {
        aabb.lower = glm::min(aabb.lower, triangle.V0.pos);
        aabb.lower = glm::min(aabb.lower, triangle.V1.pos);
        aabb.lower = glm::min(aabb.lower, triangle.V2.pos);

        aabb.upper = glm::max(aabb.upper, triangle.V0.pos);
        aabb.upper = glm::max(aabb.upper, triangle.V1.pos);
        aabb.upper = glm::max(aabb.upper, triangle.V2.pos);
    }

    const float eps = 1e-4;
    aabb.lower -= glm::vec3(eps);
    aabb.upper += glm::vec3(eps);

    return aabb;
}

void MeshObject::setTransform(Transformation transformation) {
    mTransform = transformation;
}

Transformation MeshObject::getTransform() {
    return mTransform;
}

Transformation &MeshObject::getTransformRef() {
    return mTransform;
}
