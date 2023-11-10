#pragma once
#include "glm/glm.hpp"

struct Vertex
{
    alignas(16) glm::vec3 pos{};
    alignas(16) glm::vec3 normal{};
    alignas(16) glm::vec2 uv{};
};

struct Material
{
    alignas(16) glm::vec4 color{};
    alignas(16) glm::vec4 boolsSpecTrans{}; // specular if x comp is 1, transmissive if y comp is 1

    explicit Material() {
        color = glm::vec4(0);
        boolsSpecTrans = glm::vec4(0);
    }

    explicit Material(glm::vec4 color) {
        this->color = color;
        boolsSpecTrans = glm::vec4(0);
    }
};

struct Triangle
{
    Vertex V0;
    Vertex V1;
    Vertex V2;

    Material material{};
};

struct BufferMeshObject {
    unsigned int firstTriangleIndex;
    unsigned int triangleCount;
    unsigned int emissive; // 1 if emissive, 0 else
};