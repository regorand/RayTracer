#include "res/shaders/material/material.glsl"

struct Vertex {
    vec4 pos;
    vec4 normal;
    vec4 uv;
};

/*
struct Material {
    vec4 color;
};
*/

struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    MaterialTMP mat;
};

struct AABB {
    vec4 lower;
    vec4 upper;
};

struct MeshObject {
    mat4 invTransform;
    mat4 normalMat;
    AABB aabb;
    uint firstTriangleIndex;
    uint triangleCount;
    uint emissive; // 1 if emissive, 0 else
    uint dummy;
};
