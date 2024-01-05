#pragma once

#include <vector>
#include "glWrappers/shaders/SSBO.h"
#include "renderers/Types.h"

class SceneBuffers {
private:
    SSBO mTriangleSSBO;
    SSBO mObjectSSBO;
    SSBO mMaterialSSBO;
    SSBO mEmissiveSSBO;

public:
    bool createGPUBuffers(std::vector<Triangle> &triangles,
                          std::vector<BufferMeshObject> &objectIds,
                          std::vector<MaterialTMP> &materials,
                          std::vector<unsigned int> &emissiveObjects);

    void bind();
    void unbind();

    bool isValid();

};
