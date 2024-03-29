#pragma once

#include <string>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

typedef struct material_s {
    std::string name {};
    glm::vec4 albedo {};
    glm::vec3 emissionColor {};
    float roughness {};
    float metallic {};
    float transparency {};
    float emission {};

    material_s() = default;
} Material;