#pragma once

#include <string>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

typedef struct buf_material_s {
    glm::vec4 albedo {};
    glm::vec4 emission {};
    float roughness {1.0f};
    float metallic {};
    float transparency {};
    float IOR;
} BufferMaterial;

typedef struct material_s {
    std::string name {};
    BufferMaterial bufferMaterial {};

    material_s() = default;
} Material;