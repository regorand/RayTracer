#pragma once

#include "renderers/Types.h"
#include "glWrappers/ShaderStorageBuffer.h"
#include "SceneTypes.h"
#include "Material.h"

// TODO move this into own file

typedef struct SceneBuffers {
    std::vector<RasterVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Mesh> meshes;
    std::vector<glm::mat4> transforms;
    std::vector<unsigned int> emissives;
    std::vector<Material> materials;
    std::vector<TriangleNew> triangles;
} SceneBuffers;