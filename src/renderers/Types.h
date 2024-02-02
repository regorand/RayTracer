#pragma once

#include "glm/glm.hpp"

struct Vertex {
    alignas(16) glm::vec3 pos{};
    alignas(16) glm::vec3 normal{};
    alignas(16) glm::vec2 uv{};
};

/*
struct Material
{
    alignas(16) glm::vec4 color{};
    // alignas(16) glm::vec4 boolsSpecTrans{}; // specular if x comp is 1, transmissive if y comp is 1

    explicit Material() {
        color = glm::vec4(0);
        // boolsSpecTrans = glm::vec4(0);
    }

    explicit Material(glm::vec4 color) {
        this->color = color;
        // boolsSpecTrans = glm::vec4(0);
    }
};
 */

inline bool sameColor(glm::vec4 color1, glm::vec4 color2) {
    return glm::length(color1 - color2) < 1e-3;
}

struct Material {
    glm::vec4 albedo;
    glm::vec4 emissionColor;
    float roughness;
    float metallic;
    float transparency;
    float emission;
    // TODO
    // IOR, specular tint, etc.

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material
    // sample metallic value to decide if to evalutate metallic or dielectric bsdf ?
    // -> need to consider this in pdf -> is that just a constant factor ?
    // need sheen layer for colorless highlights ?
    // metallic seems to have higher prio than transmittance of dielectric
    // -> look at principled bsdf shader example!

    bool isSame(const Material &other) const {
        return sameColor(this->albedo, other.albedo)
               && sameColor(this->emissionColor, other.emissionColor)
               && this->roughness - other.roughness < 1e-3
               && this->metallic - other.metallic < 1e-3
               && this->transparency - other.transparency < 1e-3
               && this->emission - other.emission < 1e-3;
    }

    friend inline bool operator==(const Material &left, const Material &right) {
        left.isSame(right);
    }

    friend inline bool operator!=(const Material &left, const Material &right) { return !(left == right); }
};

struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    uint32_t materialIndex;
    Material material{};
};

struct AABB {
    alignas(16) glm::vec3 lower{};
    alignas(16) glm::vec3 upper{};
};

struct BufferMeshObject {
    glm::mat4x4 invTransform{glm::mat4x4(1)};
    glm::mat4x4 normalMat{glm::mat4x4(1)};

    AABB aabb{}; // AABB given in object space -> transform ray into object space
    alignas(4) unsigned int firstTriangleIndex{};
    alignas(4) unsigned int triangleCount{};
    alignas(4) unsigned int emissive{}; // 1 if emissive, 0 else
    alignas(4) unsigned int dummy{};
};
