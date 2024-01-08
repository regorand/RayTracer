
#include "MeshObject.h"
#include "glm/ext/matrix_transform.hpp"
#include "deviceShared/MaterialFlags.h"
#include <array>

void MeshObject::addTriangle(Triangle triangle) {
    mTriangles.push_back(triangle);
}

std::vector<Triangle> &MeshObject::getTriangleBuffer() {
    return mTriangles;
}

AABB MeshObject::createAABB() {
    AABB aabb{glm::vec3(INFINITY), glm::vec3(-INFINITY)};

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

std::vector<Triangle> &MeshObject::getTriangles() {
    return mTriangles;
}

void MeshObject::createRasterBuffers() {
    std::vector<glm::vec3> data; // use the fact that all needed attributes are vec3

    std::vector<unsigned int> indices;

    unsigned int vertexCount = 0;

    for (auto &triangle: mTriangles) {
        std::array<Vertex, 3> vertices = {triangle.V0, triangle.V1, triangle.V2};

        glm::vec3 edge1 = triangle.V1.pos - triangle.V0.pos;
        glm::vec3 edge2 = triangle.V2.pos - triangle.V0.pos;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        for (auto &vertex: vertices) {
            data.emplace_back(vertex.pos);
            data.emplace_back(normal);

            // TODO display different materials differently (see commented out code before)
            data.emplace_back(triangle.material.albedo);

            /*
            unsigned int compValue = reinterpret_cast<unsigned int &>(triangle.material.color.w);
            if (compValue == 0) {
            } else if ((compValue & EMISSIVE_FLAG) != 0) {
                data.emplace_back(glm::vec4(1, 1, 1, 0));
            } else if ((compValue & TRANSMISSIVE_FLAG) != 0) {
                data.emplace_back(glm::vec4(0.8) * glm::vec4(1, 1, 1, 0));
            } else {
                data.emplace_back(glm::vec4(0.6) * glm::vec4(1, 1, 1, 0));
            }
             */

        }

        indices.push_back(vertexCount++);
        indices.push_back(vertexCount++);
        indices.push_back(vertexCount++);
    }

    mVa.bind();

    mAb = ArrayBuffer(data.data(), sizeof(glm::vec3) * data.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), 0); // pos
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void *) sizeof(glm::vec3)); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void *) (2 * sizeof(glm::vec3))); // color
    glEnableVertexAttribArray(2);

    mIb = IndexBuffer(indices.data(), indices.size());

}

bool MeshObject::canRasterize() {
    return mVa.isValid() && mAb.isValid() && mIb.isValid();
}

VertexArray &MeshObject::getVertexArray() {
    return mVa;
}

IndexBuffer &MeshObject::getIndexBuffer() {
    return mIb;
}

bool MeshObject::isTransmissive() {
    if (mTriangles.empty()) return false;
    return mTriangles.front().material.transparency > 0.99f;
}

bool MeshObject::isEmissive() {
    return mIsEmissive;
    /*
    if (mTriangles.empty()) return false;
    return mTriangles.front().material.emission;
     */
}
