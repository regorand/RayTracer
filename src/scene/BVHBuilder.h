#pragma once
// based on https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
#include "glm/glm.hpp"

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



    void buildBVH();


};
