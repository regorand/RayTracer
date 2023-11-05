#include "res/shaders/utils/MeshTypes.glsl"

layout(std430, binding = TRIANGLE_BUFFER_BINDING) buffer triangleList
{
    Triangle triangles[];
};


layout(std430, binding = OBJECT_BUFFER_BINDING) buffer objectList
{
    uint objectIndices[];
};


layout(std430, binding = MATERIAL_BUFFER_BINDING) buffer materialList
{
    Material materials[];
};


layout(std430, binding = EMISSIVE_BUFFER_BINDING) buffer emissiveList
{
    uint emissiveIndices[];
};

/*
    TODO for first version needed:
        1. Triangle buffer
        2. Object buffer
        3. Material buffer
        4. Light indices buffer
*/