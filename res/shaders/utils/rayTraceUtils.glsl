#include "res/shaders/utils/typesAndValues.glsl"
#include "res/shaders/utils/intersectUtils.glsl"
#include "src/deviceShared/BufferBindings.h"

/*
bool getClosestHit2(Ray ray, out RayHit rayhit) {

}
*/

bool getClosestHit(Ray ray, out RayHit rayhit) {
    float closest = 1000000;
    bool anyHit = false;

    // Spheres
    for (int i = 0; i < NUM_SPHERES; i++) {
        float dist;
        bool tmpEM;
        Sphere sphere = spheres[i];
        bool hit = intersectSphere(ray, sphere, dist, tmpEM);
        if (hit && dist > 0) {
            anyHit = true;
            if (dist < closest) {
                closest = dist;
                rayhit.dist = dist;
                rayhit.mat = sphere.mat;
                rayhit.position = ray.origin + ray.direction * dist;
                rayhit.normal = normalize(rayhit.position - sphere.center);
                rayhit.enteringMat = tmpEM;
            }
        }
    }

    // Triangles
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        float dist;
        bool tmpEM;
        Triangle triangle = triangles[i];
        bool hit = intersectTriangle(ray, triangle, dist, tmpEM);
        if (hit && dist > 0) {
            anyHit = true;
            if (dist < closest) {
                closest = dist;
                rayhit.dist = dist;
                rayhit.mat = triangle.mat;
                rayhit.position = ray.origin + ray.direction * dist;
                rayhit.normal = normalize(cross(triangle.P0 - triangle.P1, triangle.P0 - triangle.P2));
                rayhit.enteringMat = tmpEM;
            }
        }
    }

    return anyHit;
}



bool getClosestHit(Ray ray, out Sphere closestSphere, out float dist, out bool enteringMaterial) {
    float closest = 1000000;
    bool anyHit = false;
    int closestIndex = -1;

    // Spheres
    for (int i = 0; i < 5; i++) {
        float dist;
        bool tmpEM;
        bool hit = intersectSphere(ray, spheres[i], dist, tmpEM);
        if (hit && dist > 0) {
            enteringMaterial = tmpEM;
            anyHit = true;
            if (dist < closest) {
                closest = dist;
                closestIndex = i;
            }
        }
    }

    /*

    Triangle triangle;
    Material mat;
    mat.color = vec3(1, 0, 0);
    mat.diffuse = true;
    mat.emissive = false;
    mat.transmissive = false;
    mat.IOR = 1.0f;
    triangle.P0 = vec3(10, -10, -15);
    triangle.P1 = vec3(-10, 10, -15);
    triangle.P2 = vec3(-10, -10, -15);
    triangle.mat;
    float dist;
    bool tmpEM;
    intersectTriangle(ray, triangle, dist, tmpEM);
    if (hit && dist < closest) {
        closest = dist;
    }
*/


    if (anyHit) {
        closestSphere = spheres[closestIndex];
        dist = closest;
    }
    return anyHit;
}

vec3 sampleNEETarget(vec2 seed, out Triangle triangle) {
    // terrible name, but it indexes into the array of indices
    int sampledIndexIndex = int(max(rand(seed) * LIGHT_SOURCE_COUNT - 1e-5, 0.0f)); // complicated to make 100% sure index can not be the same as LIGHT_SOURCE_COUNT

    int primitiveIndex = LIHGT_INDICES[sampledIndexIndex];

    triangle = triangles[primitiveIndex];
    return sampleTrianglePos(triangle, seed);
}