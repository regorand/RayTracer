#include "Mesh.h"

#include <utility>

Mesh::Mesh(std::string name)
    : mName(std::move(name))
{}

void Mesh::addPrimitive(const MeshPrimitive& primitive) {
    mPrimitives.push_back(primitive);
}

size_t Mesh::getPrimitveCount() {
    return mPrimitives.size();
}

void Mesh::setIndexOffset(int offset) {
    indexOffset = offset;
}

void Mesh::setIndexCount(int newCount) {
    indexCount = newCount;
}

int Mesh::getIndexOffset() const {
    return indexOffset;
}

int Mesh::getIndexCount() const {
    return indexCount;
}

int Mesh::fillBuffers(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, int vertexOffset) {
    int indicesAdded = 0;

    for (auto &primitive: mPrimitives) {
        indicesAdded += primitive.fillBuffers(vertices, indices, vertexOffset);
    }

    return indicesAdded;
}
