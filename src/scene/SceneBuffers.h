#pragma once

#include "glWrappers/ShaderStorageBuffer.h"
#include "SceneTypes.h"
#include "Material.h"
#include "glm/mat4x4.hpp"

typedef struct CPUSceneBuffers {
    // General Buffers
    std::vector<Object> objects;
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<Material> materials;


    // Rasterizing dataStructures
    std::vector<RasterVertex> vertices;
    std::vector<unsigned int> indices;

    // Path Tracing Buffers
    std::vector<unsigned int> emissives;
    std::vector<Triangle> triangles;
} CPUSceneBuffers;

typedef struct {
    ShaderStorageBuffer triangleBuffer;
    ShaderStorageBuffer objectBuffer;
    ShaderStorageBuffer meshBuffer;
    ShaderStorageBuffer materialBuffer;
    ShaderStorageBuffer transformationBuffer;
    ShaderStorageBuffer emissiveBuffer;
} GPUSceneBuffers;