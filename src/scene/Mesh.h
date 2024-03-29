#pragma once

#include <vector>
#include <string>
#include "renderers/Types.h"
#include "MeshPrimitive.h"

class Mesh {
    std::string mName;

    std::vector<MeshPrimitive> mPrimitives;

    // Index into openGl Index buffer, where this meshes triangles begin
    int indexOffset;
    int indexCount; // Count of indices in this mesh, equal to 3 x triangle count

public:
    Mesh(std::string name);

    void addPrimitive(const MeshPrimitive& primitive);

    size_t getPrimitveCount();

    void setIndexOffset(int offset);
    void setIndexCount(int newCount);

    int getIndexOffset() const;
    int getIndexCount() const;

    int fillBuffers(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, int vertexOffset);
};
