#pragma once

typedef struct SceneObject {
    int baseIndex; // Where in vertex buffer does it start
    int indexCount; // How many indices in vertex buffer belong to this, starting from baseIndex
    AABB aabb; // AABB, probably not needed anymore after BVH
    int isEmissive; // temp
    glm::mat4 transform; // object -> world transform, wont be needed for rendering once bvh is built
} SceneObject;

// TODO another struct for rasterizing -> needs material in vertex, not triangle (i think)
typedef struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
} Vertex;

typedef struct RasterVertex {
    Vertex vertex;
    int materialId;
} RasterVertex;

typedef struct TriangleNew {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    int materialIndex;
} TriangleNew;

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