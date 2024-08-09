#pragma once

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

typedef struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
} Vertex;

typedef struct RasterVertex {
    Vertex vertex;
    int materialId;
} RasterVertex;

typedef struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    int materialIndex;
} Triangle;

typedef struct {
    glm::vec3 lower;
    glm::vec3 upper;
} AABB;

struct BufferMesh {
    AABB aabb {}; // AABB given in object space -> transform ray into object space
    alignas(4) unsigned int firstTriangleIndex {};
    alignas(4) unsigned int triangleCount {};
    alignas(4) unsigned int emissive {}; // 1 if emissive, 0 else
    alignas(4) unsigned int transformIndex {};
};

struct Mesh {
    BufferMesh bufferMesh;
    unsigned int indexOffset {};
    unsigned int indexCount {};
};