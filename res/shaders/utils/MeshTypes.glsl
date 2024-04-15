#include "res/shaders/material/material.glsl"

struct Vertex {
    vec3 pos;
    vec3 normal;
    uint materialId;
    // vec2 uv;
};

struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;
};

vec3 triangleNormal(Triangle triangle) {
    vec3 e1 = normalize(triangle.V1.pos.xyz - triangle.V0.pos.xyz);
    vec3 e2 = normalize(triangle.V2.pos.xyz - triangle.V0.pos.xyz);
    return normalize(cross(e1, e2));
}

float triangleArea(Triangle triangle) {
    vec3 e1 = triangle.V1.pos.xyz - triangle.V0.pos.xyz;
    vec3 e2 = triangle.V2.pos.xyz - triangle.V0.pos.xyz;
    float l1 = length(e1);
    float l2 = length(e2);

    float theta = acos(dot((e1 / l1), (e2 / l2)));
    float sinTheta = sin(theta);

    return sinTheta * l1 * l2 / 2;
}

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
