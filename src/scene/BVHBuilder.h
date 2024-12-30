#pragma once
// based on https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
#include <vector>
#include "glm/glm.hpp"
#include "SceneTypes.h"

typedef struct BVHNode {
    glm::vec3 aabbLower;
    glm::vec3 aabbUpper;
    unsigned int leftFirst; // index of left child node or index of first triangle depending on triCount
    unsigned int triCount;
} BVHNode;

typedef struct BVH {
    unsigned int rootNodeIndex;
} BVH;


class BVHBuilder {
private:
    // Absolute max depth, enforced because in shader I will need a stack for traversing recursive structure, need some max to preallocate it
    constexpr static unsigned int DEPTH_LIMIT = 32;

    std::vector<unsigned int> triIndices;

    std::vector<BVHNode> bvhNodes;

    std::vector<Mesh> &meshes;
    std::vector<Triangle> &triangles;

    int maxDepth;

public:
    BVHBuilder(std::vector<Mesh> &meshes, std::vector<Triangle> &triangles);

    void build();

private:
    void buildMeshBVH(int meshIndex);

    void subdivide(int nodeIdx, int depth);

    void updateNodeBounds(int nodeIdx);

    void swapIndices(int i, int j);

    void assertNodeIdx(int index);
};
