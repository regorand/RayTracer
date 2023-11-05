#pragma once
#include "glm/glm.hpp"

struct Vertex
{
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 normal;
    alignas(16) glm::vec2 uv;
};

struct Material
{
    alignas(16) glm::vec3 color;
};

struct Triangle
{
    Vertex V0;
    Vertex V1;
    Vertex V2;

    alignas(16) glm::vec3 color;
};