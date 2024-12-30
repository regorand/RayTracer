#include <stdexcept>
#include <stack>
#include <iostream>
#include "BVHBuilder.h"

BVHBuilder::BVHBuilder(std::vector<Mesh> &meshes, std::vector<Triangle> &triangles)
    : meshes(meshes), triangles(triangles)
{ }

// TODO might be useful to add an offset from outside to it, so multiple meshes can store their bvhs in same buffer without extra offset
void BVHBuilder::build() {
    triIndices.resize(triangles.size());

    for (int i = 0; i < triIndices.size(); i++) {
        triIndices[i] = i;
    }

    for (int i = 0; i < meshes.size(); i++) {
        buildMeshBVH(i);
    }
}

/* TODO BVH
    1. build first simple version of bvh
        1.1 Maybe somehow confirm correctness through debugger
    2. Build a buffer for it
    3. Write a shader that displays BVH (not fully rendering yet)
        -> probably create new Renderer for it
    4. Render using BVH
    5. Improve BVH building
 */

void BVHBuilder::buildMeshBVH(int meshIndex) {
    Mesh &mesh = meshes[meshIndex];

    mesh.bufferMesh.bvhRoot = bvhNodes.size();
    auto& root = bvhNodes.emplace_back();


    root.aabbLower = mesh.bufferMesh.aabb.lower;
    root.aabbUpper = mesh.bufferMesh.aabb.upper;

    root.leftFirst = 0;
    root.triCount = mesh.bufferMesh.triangleCount;

    maxDepth = 1;
    subdivide(mesh.bufferMesh.bvhRoot, 2);

    /*
    struct stackStruct {
        unsigned int index;
        unsigned int depth;
    };
    std::stack<stackStruct> stack;

    stack.push({ bvh.rootNodeIndex, 0} );
    int totalPrims = 0;
    int depth = 0;

    while(!stack.empty()) {
        stackStruct sStruct = stack.top();
        BVHNode &node = bvhNodes[sStruct.index];
        stack.pop();

        bool isLeaf = node.triCount > 0;

        for (int i = 0; i < sStruct.depth; i++) std::cout << "\t";
        if (isLeaf) {
            std::cout << "Found Leaf with index " << sStruct.index << " and " << node.triCount << " primitives" << std::endl;
            totalPrims += node.triCount;
        } else {
            std::cout << "Found inner with index " << sStruct.index << std::endl;
            stack.push({ node.leftFirst, sStruct.depth + 1});
            stack.push({ node.leftFirst + 1, sStruct.depth + 1 });
        }
    }
    std::cout << "Total Prims: " << totalPrims << std::endl;
     */
}

void BVHBuilder::subdivide(int nodeIdx, int depth) {
    assertNodeIdx(nodeIdx);
    BVHNode &node = bvhNodes[nodeIdx];

    if (node.triCount <= 2 || depth > DEPTH_LIMIT) return;

    glm::vec3 extents = node.aabbUpper - node.aabbLower;

    int axis = depth % 3;
    /*
    int axis = 0;
    if (extents.y > extents[axis]) axis = 1;
    if (extents.z > extents[axis]) axis = 2;
    */

    float splitPos = node.aabbLower[axis] + extents[axis] * 0.5f;

    int i = node.leftFirst;
    int j = i + node.triCount - 1;

    while (i <= j) {
        if (triangles[triIndices[i]].center[axis] <= splitPos) {
            // triangle belongs to the left of splitPos
            i++;
        } else {
            // triangle belong to the right of splitPos
            swapIndices(i, j);
            j--;
        }
    }

    int leftCount = i - node.leftFirst;
    // Splitting doesn't do anything here
    if (leftCount == 0 || leftCount == node.triCount) return;

    maxDepth = std::max(maxDepth, depth);

    int newLeftIndex = bvhNodes.size();

    BVHNode &left = bvhNodes.emplace_back();
    left.leftFirst = node.leftFirst;
    left.triCount = leftCount;

    BVHNode &right = bvhNodes.emplace_back();
    right.leftFirst = i;
    right.triCount = node.triCount - leftCount;

    // Access through index again, emplacing new nodes might have changed internals of vector
    bvhNodes[nodeIdx].triCount = 0;
    bvhNodes[nodeIdx].leftFirst = newLeftIndex;

    updateNodeBounds(newLeftIndex);
    updateNodeBounds(newLeftIndex + 1);

    subdivide(newLeftIndex, depth + 1);
    subdivide(newLeftIndex + 1, depth + 1);
}

void BVHBuilder::updateNodeBounds(int nodeIdx) {
    assertNodeIdx(nodeIdx);
    BVHNode &node = bvhNodes[nodeIdx];

    if (node.triCount == 0) {
        updateNodeBounds(node.leftFirst);
        updateNodeBounds(node.leftFirst + 1);

        node.aabbLower = glm::min(bvhNodes[node.leftFirst].aabbLower, bvhNodes[node.leftFirst + 1].aabbLower);
        node.aabbUpper = glm::max(bvhNodes[node.leftFirst].aabbUpper, bvhNodes[node.leftFirst + 1].aabbUpper);

        return;
    }

    node.aabbLower = glm::vec3( 1e30);
    node.aabbUpper = glm::vec3(-1e30);
    for (int i = 0; i < node.triCount; i++) {
        node.aabbLower = glm::min(node.aabbLower, triangles[triIndices[node.leftFirst + i]].V0.position);
        node.aabbLower = glm::min(node.aabbLower, triangles[triIndices[node.leftFirst + i]].V1.position);
        node.aabbLower = glm::min(node.aabbLower, triangles[triIndices[node.leftFirst + i]].V2.position);

        node.aabbUpper = glm::max(node.aabbUpper, triangles[triIndices[node.leftFirst + i]].V0.position);
        node.aabbUpper = glm::max(node.aabbUpper, triangles[triIndices[node.leftFirst + i]].V1.position);
        node.aabbUpper = glm::max(node.aabbUpper, triangles[triIndices[node.leftFirst + i]].V2.position);
    }
}

void BVHBuilder::swapIndices(int i, int j) {
    int tmp = triIndices[i];
    triIndices[i] = triIndices[j];
    triIndices[j] = tmp;
}

void BVHBuilder::assertNodeIdx(int index) {
    if (index >= bvhNodes.size()) {
        throw std::runtime_error("Invalid node Index");
    }
}
