
#include "SceneBuffers.h"
#include "deviceShared/BufferBindings.h"

SceneBuffers::SceneBuffers(std::vector<Triangle> &triangles,
                           std::vector<unsigned int> &objectIds,
                           std::vector<Material> &materials,
                           std::vector<unsigned int> &emissiveObjects)
        : mTriangleSSBO(
        SSBO(triangles.data(), sizeof(Triangle) * triangles.size(), GL_STATIC_READ, TRIANGLE_BUFFER_BINDING)),
          mObjectSSBO(SSBO(objectIds.data(), sizeof(unsigned int) * objectIds.size(), GL_STATIC_READ,
                           OBJECT_BUFFER_BINDING)),
          mMaterialSSBO(
                  SSBO(materials.data(), sizeof(Material) * materials.size(), GL_STATIC_READ, MATERIAL_BUFFER_BINDING)),
          mEmissiveSSBO(SSBO(emissiveObjects.data(), sizeof(unsigned int) * emissiveObjects.size(), GL_STATIC_READ,
                             EMISSIVE_BUFFER_BINDING)) {}

void SceneBuffers::bind() {
    mTriangleSSBO.bind();
    mObjectSSBO.bind();
    mMaterialSSBO.bind();
    mEmissiveSSBO.bind();
}

void SceneBuffers::unbind() {
    mTriangleSSBO.unbind();
    mObjectSSBO.unbind();
    mMaterialSSBO.unbind();
    mEmissiveSSBO.unbind();
}
