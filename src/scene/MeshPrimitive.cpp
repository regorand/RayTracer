
#include "MeshPrimitive.h"
#include "Vertex.h"
#include <utility>

MeshPrimitive::MeshPrimitive(std::vector<VertexOld> vertices, std::vector<unsigned int> indices, unsigned int materialIndex)
    : mVertices(std::move(vertices)), mIndices(std::move(indices)), mMaterialIndex(materialIndex)
{ }

size_t MeshPrimitive::vertexCount() {
    return mVertices.size();
}

size_t MeshPrimitive::indexCount() {
    return mIndices.size();
}

std::vector<VertexOld> &MeshPrimitive::getVertices() {
    return mVertices;
}

std::vector<unsigned int> &MeshPrimitive::getIndices() {
    return mIndices;
}

unsigned int MeshPrimitive::getMaterialIndex() {
    return mMaterialIndex;
}

int MeshPrimitive::fillBuffers(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, int &vertexOffset) {
    for (auto &rawVertex: mVertices) {
        Vertex &vertex = vertices.emplace_back();
        vertex.position = rawVertex.pos;
        vertex.normal = rawVertex.normal;
        vertex.materialIndex = mMaterialIndex;
    }

    for (auto& index: mIndices) {
        indices.push_back(index + vertexOffset);
    }

    return mIndices.size();
}

