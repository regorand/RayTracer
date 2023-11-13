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

bool intersectAABB(Ray ray, AABB aabb, out float t) {
    // AABB intersection code taken from
    // https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms/18459#18459

    // r.dir is unit direction vector of ray
    vec3 dirfrac;

    dirfrac.x = 1.0f / ray.direction.x;
    dirfrac.y = 1.0f / ray.direction.y;
    dirfrac.z = 1.0f / ray.direction.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    vec3 lb = aabb.lower.xyz;
    vec3 rt = aabb.upper.xyz;

    float t1 = (lb.x - ray.origin.x)*dirfrac.x;
    float t2 = (rt.x - ray.origin.x)*dirfrac.x;
    float t3 = (lb.y - ray.origin.y)*dirfrac.y;
    float t4 = (rt.y - ray.origin.y)*dirfrac.y;
    float t5 = (lb.z - ray.origin.z)*dirfrac.z;
    float t6 = (rt.z - ray.origin.z)*dirfrac.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        t = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        t = tmax;
        return false;
    }

    t = tmin;
    return true;
}

bool intersectObject(Ray ray, MeshObject object, out RayHit rayhit) {
    // TODO could try to intersect bounding box first here before looping triangles

    // TODO once objects have transformations, transform the ray into object space and perform intersection there
        // TODO difficulty with this is, that the t value needs to be transformed somehow (i think)

    uint objectBaseIndex = object.firstTriangleIndex;

    float closest = 1000000;

    float dist;

    bool aabbHit = intersectAABB(ray, object.aabb, dist);
    if (!aabbHit) return false;

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