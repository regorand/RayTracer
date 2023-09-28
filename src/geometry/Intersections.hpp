
#ifndef RAYTRACER_INTERSECTIONS_HPP
#define RAYTRACER_INTERSECTIONS_HPP

#include "Objects.h"
#include "glm/geometric.hpp"

bool intersectSphere(Ray ray, Sphere sphere, float *dist) {
    glm::vec3 centerMinusOrigin = ray.origin - sphere.center;

    float a = glm::dot(ray.direction, ray.direction);
    float b = 2 * glm::dot(ray.direction, centerMinusOrigin);
    float c = glm::dot(centerMinusOrigin, centerMinusOrigin) - sphere.radius * sphere.radius;

    float discriminant = b * b - (4 * a * c);
    if (discriminant < 0) return false;
    float discrRoot = glm::sqrt(discriminant);

    float t1 = (-b + discrRoot) / (2 * a);
    float t2 = (-b - discrRoot) / (2 * a);

    float t_near = glm::min(t1, t2);
    float t_far = glm::max(t1, t2);
    if (t_near >= 0) {
        *dist = t_near;
        return true;
    }
    if (t_far < 0) return false;
    *dist = t2;
    return true;
}

#endif //RAYTRACER_INTERSECTIONS_HPP
