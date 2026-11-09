#include "res/shaders/utils/MeshTypes.glsl"
#include "src/deviceShared/BufferBindings.h"

uniform uint objectCount;
uniform uint emissiveCount;

layout(std430, binding = TRIANGLE_BUFFER_BINDING) buffer triangleList
{
    Triangle triangles[];
};


layout(std430, binding = OBJECT_BUFFER_BINDING) buffer objectList
{
    Object objects[];
};


layout(std430, binding = MESH_BUFFER_BINDING) buffer meshList
{
    Mesh meshes[];
};


layout(std430, binding = MATERIAL_BUFFER_BINDING) buffer materialList
{
    Material materials[];
};


layout(std430, binding = EMISSIVE_BUFFER_BINDING) buffer emissiveList
{
    uint emissiveIndices[];
};

layout(std430, binding = TRANSFORM_BUFFER_BINDING) buffer transformList
{
    mat4 transforms[];
};

layout(std430, binding = BVH_NODES_BUFFER_BINDING) buffer bvhNodeList
{
    BVHNode bvhNodes[];
};

layout(std430, binding = TRIANGLE_INDICES_BUFFER_BINDING) buffer triIndexList
{
    uint triIndices[];
};