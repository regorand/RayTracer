#pragma once

#include "glm/vec3.hpp"

typedef struct vertex_s {
    glm::vec3 position;
    glm::vec3 normal;
    int materialIndex;
} Vertex;