#pragma once

#include "glWrappers/ShaderStorageBuffer.h"
#include "SceneTypes.h"
#include "Material.h"
#include "glm/mat4x4.hpp"

typedef struct CPUSceneBuffers {
    std::vector<RasterVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<unsigned int> emissives;
    std::vector<Material> materials;
    std::vector<Triangle> triangles;
} CPUSceneBuffers;

typedef struct {
    ShaderStorageBuffer triangleBuffer;
    ShaderStorageBuffer meshBuffer;
    ShaderStorageBuffer materialBuffer;
    ShaderStorageBuffer transformationBuffer;
    ShaderStorageBuffer emissiveBuffer;
} GPUSceneBuffers;