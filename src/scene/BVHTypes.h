#pragma once

typedef struct BVHNode {
    // vectors and uints interleaved because it was the easiest hack to make the buffer alignment between gpu and cpu work
    // I think this should work according to glsl spec now
    // Dont want to introduce dummy elements to make sure node is 32 byte
    glm::vec3 aabbLower;
    unsigned int leftFirst; // index of left child node or index of first triangle depending on triCount

    glm::vec3 aabbUpper;
    unsigned int triCount;
} BVHNode;

typedef struct BVH {
    unsigned int rootNodeIndex;
} BVH;