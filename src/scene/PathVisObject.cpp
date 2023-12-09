
#include "PathVisObject.h"

PathVisObject::PathVisObject(std::vector<glm::vec3> vertices)
    : mVertices(vertices)
{ }

void PathVisObject::createRasterBuffers() {
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < mVertices.size() - 1; i++) {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    mVa.bind();

    mAb = ArrayBuffer(mVertices.data(), sizeof(glm::vec3) * mVertices.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // pos
    glEnableVertexAttribArray(0);

    mIb = IndexBuffer(indices.data(), indices.size());
}

bool PathVisObject::canRasterize() {
    return mVa.isValid() && mAb.isValid() && mIb.isValid();
}

VertexArray &PathVisObject::getVertexArray() {
    return mVa;
}

IndexBuffer &PathVisObject::getIndexBuffer() {
    return mIb;
}
