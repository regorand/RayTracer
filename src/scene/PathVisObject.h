#pragma once

#include <vector>
#include "glm/vec3.hpp"
#include "glWrappers/VertexArray.h"
#include "glWrappers/ArrayBuffer.h"
#include "glWrappers/IndexBuffer.h"

class PathVisObject {
    std::vector<glm::vec3> mVertices;

    VertexArray mVa;
    ArrayBuffer mAb;
    IndexBuffer mIb;

public:
    PathVisObject(std::vector<glm::vec3> vertices);

    void createRasterBuffers();

    bool canRasterize();

    VertexArray &getVertexArray();
    IndexBuffer &getIndexBuffer();

    bool hasVertices();
};