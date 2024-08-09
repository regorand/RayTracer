
#pragma once

#include <vector>
#include "renderers/Types.h"
#include "Vertex.h"

/*
  * Primitive here refers to what glTf means by a mesh primitive
  */
class MeshPrimitive {

private:
    std::vector<VertexOld> mVertices;
    std::vector<unsigned int> mIndices;
    int mMaterialIndex;

public:
    MeshPrimitive(std::vector<VertexOld> vertices, std::vector<unsigned int> indices, int mMaterialIndex);

    size_t vertexCount();
    size_t indexCount();

    std::vector<VertexOld> &getVertices();
    std::vector<unsigned int> &getIndices();
    int getMaterialIndex();

    BufferMeshObject buildBufferMeshObject();

    /*
     * @returns: the amount of indices added
     */
    int fillBuffers(std::vector<Vertex_Old> &vertices, std::vector<unsigned int> &indices, int &vertexOffset);


};
