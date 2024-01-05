#include <iostream>
#include "SceneBuffers.h"
#include "deviceShared/BufferBindings.h"

bool SceneBuffers::createGPUBuffers(std::vector<Triangle> &triangles, std::vector<BufferMeshObject> &objectIds,
                                    std::vector<MaterialTMP> &materials, std::vector<unsigned int> &emissiveObjects) {
    mTriangleSSBO = SSBO(triangles.data(), sizeof(Triangle) * triangles.size(), GL_STATIC_READ,
                         TRIANGLE_BUFFER_BINDING);

    mObjectSSBO = SSBO(objectIds.data(), sizeof(BufferMeshObject) * objectIds.size(), GL_STATIC_READ,
                       OBJECT_BUFFER_BINDING);

    mMaterialSSBO = SSBO(materials.data(), sizeof(MaterialTMP) * materials.size(), GL_STATIC_READ,
                         MATERIAL_BUFFER_BINDING);

    mEmissiveSSBO = SSBO(emissiveObjects.data(), sizeof(unsigned int) * emissiveObjects.size(), GL_STATIC_READ,
                         EMISSIVE_BUFFER_BINDING);

    return isValid();
}

void SceneBuffers::bind() {
    if (!isValid()) {
        std::cerr << "Can only bindToTarget GPU buffers when they are created and valid" << std::endl;
        return;
    }

    mTriangleSSBO.bind();
    mObjectSSBO.bind();
    mMaterialSSBO.bind();
    mEmissiveSSBO.bind();
}

void SceneBuffers::unbind() {
    if (!isValid()) {
        std::cerr << "Can only unbind GPU buffers when they are created and valid" << std::endl;
        return;
    }

    mTriangleSSBO.unbind();
    mObjectSSBO.unbind();
    mMaterialSSBO.unbind();
    mEmissiveSSBO.unbind();
}

bool SceneBuffers::isValid() {
    return mTriangleSSBO.hasData()
           && mObjectSSBO.hasData()
           && mMaterialSSBO.hasData()
           && mEmissiveSSBO.hasData();
}
