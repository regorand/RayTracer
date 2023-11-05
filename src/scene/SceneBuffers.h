#pragma once

#include <vector>
#include "glWrappers/shaders/SSBO.h"
#include "deviceShared/Types.h"

class SceneBuffers {
private:
    SSBO mTriangleSSBO;
    SSBO mObjectSSBO;
    SSBO mMaterialSSBO;
    SSBO mEmissiveSSBO;

public:
    SceneBuffers(std::vector<Triangle> &triangles,
                 std::vector<unsigned int> &objectIds,
                 std::vector<Material> &materials,
                 std::vector<unsigned int> &emissiveObjects);

    void bind();
    void unbind();

};
