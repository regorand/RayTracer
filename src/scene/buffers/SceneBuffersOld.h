#pragma once

#include <vector>
#include "glWrappers/shaders/SSBO.h"
#include "renderers/Types.h"

class SceneBuffersOld {
private:
    SSBO mTriangleSSBO;
    SSBO mObjectSSBO;
    SSBO mMaterialSSBO;
    SSBO mEmissiveSSBO;

public:
    bool createGPUBuffers(std::vector<Triangle> &triangles,
                          std::vector<BufferMeshObject> &objectIds,
                          std::vector<MaterialOld> &materials,
                          std::vector<unsigned int> &emissiveObjects);

    void bind();
    void unbind();

    bool isValid();

};
