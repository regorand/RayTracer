#include <iostream>
#include "SceneBuffersOld.h"
#include "deviceShared/BufferBindings.h"

bool SceneBuffersOld::createGPUBuffers(std::vector<Triangle> &triangles, std::vector<BufferMeshObject> &objectIds,
                                       std::vector<MaterialOld> &materials, std::vector<unsigned int> &emissiveObjects) {
    mTriangleSSBO = SSBO(triangles.data(), sizeof(Triangle) * triangles.size(), GL_STATIC_READ,
                         TRIANGLE_BUFFER_BINDING);

    // TODO write size of buffer at the very beginning, I think that works in openGL
    mObjectSSBO = SSBO(objectIds.data(), sizeof(BufferMeshObject) * objectIds.size(), GL_STATIC_READ,
                       OBJECT_BUFFER_BINDING);

    mMaterialSSBO = SSBO(materials.data(), sizeof(MaterialOld) * materials.size(), GL_STATIC_READ,
                         MATERIAL_BUFFER_BINDING);

    mEmissiveSSBO = SSBO(emissiveObjects.data(), sizeof(unsigned int) * emissiveObjects.size(), GL_STATIC_READ,
                         EMISSIVE_BUFFER_BINDING);

    return isValid();
}

void SceneBuffersOld::bind() {
    if (!isValid()) {
        std::cerr << "Can only bindToTarget GPU buffers when they are created and valid" << std::endl;
        return;
    }

    mTriangleSSBO.bind();
    mObjectSSBO.bind();
    mMaterialSSBO.bind();
    mEmissiveSSBO.bind();
}

void SceneBuffersOld::unbind() {
    if (!isValid()) {
        std::cerr << "Can only unbind GPU buffers when they are created and valid" << std::endl;
        return;
    }

    mTriangleSSBO.unbind();
    mObjectSSBO.unbind();
    mMaterialSSBO.unbind();
    mEmissiveSSBO.unbind();
}

bool SceneBuffersOld::isValid() {
    return mTriangleSSBO.hasData()
           && mObjectSSBO.hasData()
           && mMaterialSSBO.hasData()
           && mEmissiveSSBO.hasData();
}
