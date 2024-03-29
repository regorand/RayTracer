
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
    unsigned int mMaterialIndex;

public:
    MeshPrimitive(std::vector<VertexOld> vertices, std::vector<unsigned int> indices, unsigned int mMaterialIndex);

    size_t vertexCount();
    size_t indexCount();

    std::vector<VertexOld> &getVertices();
    std::vector<unsigned int> &getIndices();
    unsigned int getMaterialIndex();

    /*
     * @returns: the amount of indices added
     */
    int fillBuffers(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, int &vertexOffset);


};
