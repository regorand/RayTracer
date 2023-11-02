#include "res/shaders/utils/typesAndValues.glsl"

bool intersectSphere(Ray ray, Sphere sphere, out float dist, out bool enteringMaterial) {
    vec3 centerMinusOrigin = ray.origin - sphere.center;

    float a = dot(ray.direction, ray.direction);
    float b = 2 * dot(ray.direction, centerMinusOrigin);
    float c = dot(centerMinusOrigin, centerMinusOrigin) - sphere.radius * sphere.radius;

    float discriminant = b * b - (4 * a * c);
    if (discriminant < 0) return false;
    float discrRoot = sqrt(discriminant);

    float t1 = (-b + discrRoot) / (2 * a);
    float t2 = (-b - discrRoot) / (2 * a);

    float t_near = min(t1, t2);
    float t_far = max(t1, t2);
    if (t_near >= 0) {
        dist = t_near;
        enteringMaterial = true;
        return true;
    }
    if (t_far < 0) return false;
    dist = t2;
    enteringMaterial = false;
    return true;
}

bool intersectTriangle(Ray ray, Triangle triangle, out float dist, out bool enteringMaterial) {
    // algorithm modified from C++ example on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    const float eps = 1e-5;
    vec3 edge1 = triangle.P1 - triangle.P0;
    vec3 edge2 = triangle.P2 - triangle.P0;
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);

    if (abs(a) < eps) return false;

    float f = 1.0f / a;
    vec3 s = ray.origin - triangle.P0;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0) return false;

    dist = f * dot(edge2, q);

    if (dist > eps) {
        // vec3 N = cross(edge1, edge2);
        vec3 N = normalize(cross(edge1, edge2)); // TODO prob not necessary to normalize for what I want to do
        enteringMaterial = dot(N, (triangle.P0 - ray.origin)) > 0;
        return true;
    }

    return false;
}