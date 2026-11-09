#pragma once

#include "glWrappers/ShaderStorageBuffer.h"
#include "SceneTypes.h"
#include "Material.h"
#include "glm/mat4x4.hpp"
#include "BVHTypes.h"

typedef struct CPUSceneBuffers {
    // General Buffers
    std::vector<Object> objects;
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<Material> materials;

    std::vector<BVHNode> bvhNodes;

    // Rasterizing dataStructures
    std::vector<RasterVertex> vertices;
    std::vector<unsigned int> indices;

    // Path Tracing Buffers
    std::vector<unsigned int> emissives;
    std::vector<Triangle> triangles;
    std::vector<unsigned int> triangleIndices;
} CPUSceneBuffers;

typedef struct {
    ShaderStorageBuffer triangleBuffer;
    ShaderStorageBuffer triangleIndicesBuffer; // indirect indexing from bvh building

    ShaderStorageBuffer objectBuffer;
    ShaderStorageBuffer meshBuffer;
    ShaderStorageBuffer bvhBuffer;
    ShaderStorageBuffer materialBuffer;
    ShaderStorageBuffer transformationBuffer;
    ShaderStorageBuffer emissiveBuffer;
} GPUSceneBuffers;