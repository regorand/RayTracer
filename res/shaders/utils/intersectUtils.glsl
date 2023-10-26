#include "res/shaders/utils/types.glsl"

bool intersectSphere(Ray ray, Sphere sphere, out float dist) {
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
        return true;
    }
    if (t_far < 0) return false;
    dist = t2;
    return true;
}