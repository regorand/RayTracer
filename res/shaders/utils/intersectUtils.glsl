#include "res/shaders/utils/MeshTypes.glsl"
#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/scenes/CPUScene.glsl"

bool intersectTriangle(Ray ray, Triangle triangle, out RayHit rayhit) {
    // algorithm modified from C++ example on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    const float eps = 1e-5;
    vec3 edge1 = triangle.V1.pos.xyz - triangle.V0.pos.xyz;
    vec3 edge2 = triangle.V2.pos.xyz - triangle.V0.pos.xyz;
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);

    if (abs(a) < eps) return false;

    float f = 1.0f / a;
    vec3 s = ray.origin - triangle.V0.pos.xyz;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0) return false;

    rayhit.dist = f * dot(edge2, q);

    if (rayhit.dist > eps) {
        rayhit.mat = triangle.mat;
        rayhit.position = ray.origin + ray.direction * rayhit.dist;
        rayhit.normal = normalize(cross(edge1, edge2)); // TODO replace with interpolated normal for smooth shading ?
        rayhit.hitBackface = dot(rayhit.normal, (ray.origin - triangle.V0.pos.xyz)) < 0;
        return true;
    }

    return false;
}

bool intersectObject(Ray ray, MeshObject object, out RayHit rayhit) {
    // TODO could try to intersect bounding box first here before looping triangles

    // TODO once objects have transformations, transform the ray into object space and perform intersection there
        // TODO difficulty with this is, that the t value needs to be transformed somehow (i think)

    uint objectBaseIndex = object.firstTriangleIndex;

    float closest = 1000000;
    bool anyHit = false;

    RayHit tmp;
    for (int i = 0; i < object.triangleCount; i++) {
        Triangle triangle = triangles[objectBaseIndex + i];

        bool hit = intersectTriangle(ray, triangle, tmp);

        if (hit && tmp.dist > 0) {
            anyHit = true;
            if (tmp.dist < closest) {
                closest = tmp.dist;
                rayhit = tmp;
            }
        }
    }

    return anyHit;
}