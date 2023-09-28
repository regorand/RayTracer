
#ifndef RAYTRACER_OBJECTS_H
#define RAYTRACER_OBJECTS_H

#include "glm/vec3.hpp"
#include "graphics/Material.h"

typedef struct {
    glm::vec3 origin;
    glm::vec3 direction;
} Ray;

typedef struct {
    glm::vec3 center;
    float radius;

    Material material;
} Sphere;

#endif //RAYTRACER_OBJECTS_H
