#include "res/shaders/utils/MeshTypes.glsl"
#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/scenes/CPUScene.glsl"
#include "res/shaders/utils/Constants.glsl"
#include "res/shaders/utils/Stack.glsl"

bool intersectTriangle(Ray ray, Triangle triangle, out RayHit rayhit) {
    // algorithm modified from C++ example on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    vec3 edge1 = triangle.V1.pos.xyz - triangle.V0.pos.xyz;
    vec3 edge2 = triangle.V2.pos.xyz - triangle.V0.pos.xyz;
    vec3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);

    if (abs(a) < EPS_5) return false;

    float f = 1.0f / a;
    vec3 s = ray.origin - triangle.V0.pos.xyz;
    float u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);

    if (v < 0.0 || u + v > 1.0) return false;

    rayhit.dist = f * dot(edge2, q);

    if (rayhit.dist > EPS_5) {
        rayhit.matID = triangle.materialIndex;
        rayhit.position = ray.origin + ray.direction * rayhit.dist;
        rayhit.normal = normalize(cross(edge1, edge2)); // TODO replace with interpolated normal for smooth shading ?
        rayhit.hitBackface = dot(rayhit.normal, (ray.origin - triangle.V0.pos.xyz)) < 0;
        rayhit.normal *= sign(dot(rayhit.normal, (ray.origin - triangle.V0.pos.xyz)));

        return true;
    }

    return false;
}

struct AABBHitRes {
    bool hit;
    bool inside;
    float t_min;
    float t_max;
};

AABBHitRes intersectAABB(Ray ray, AABB aabb) {
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

    float t1 = (lb.x - ray.origin.x) * dirfrac.x;
    float t2 = (rt.x - ray.origin.x) * dirfrac.x;
    float t3 = (lb.y - ray.origin.y) * dirfrac.y;
    float t4 = (rt.y - ray.origin.y) * dirfrac.y;
    float t5 = (lb.z - ray.origin.z) * dirfrac.z;
    float t6 = (rt.z - ray.origin.z) * dirfrac.z;

    AABBHitRes hitRes;
    hitRes.t_min = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    hitRes.t_max = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    hitRes.hit = hitRes.t_min < hitRes.t_max;
    hitRes.inside = hitRes.hit && hitRes.t_min < 0 && hitRes.t_max > 0;

    return hitRes;

/*
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
*/
}

// assum e
bool intersectBVH(Ray ray, uint rootIdx, float closest, out RayHit rayhit) {
    Stack stack;
    stack.currentSize = 0;

    push(stack, rootIdx);
    uint count = 0;

    bool anyHit = false;
    RayHit tmp;
    while (stack.currentSize > 0) {
        uint nodeIdx = ptop(stack);

        BVHNode node = bvhNodes[nodeIdx];

        AABB aabb;
        aabb.lower = vec4(node.lower, 1);
        aabb.upper = vec4(node.upper, 1);

        AABBHitRes hitRes = intersectAABB(ray, aabb);

    /*
         TODO what to check in what order
            1. if not hit -> done
            3. check_t = max(t_min, 0)
            4. if check_t >= closest -> done
            5. else ->  if triCount > 0 -> intersect, store new nearset
                        else add child nodes

        -> Should work like this

        Important to note
        External call must scale distances before passing them here
        */
        // Don't hit aabb, no need to check its children and primitives
        if (!hitRes.hit) continue;

        float check_t = max(hitRes.inside ? hitRes.t_max : hitRes.t_min, 0);

        // Have a surface hit already, that is closer than the first point in aabb along ray, no need to check
        if (check_t >= closest) continue;

        if (node.triCount == 0) {
            push(stack, node.leftFirst);
            push(stack, node.leftFirst + 1);
        } else {
            for (int i = 0; i < node.triCount; i++) {
                Triangle tri = triangles[triIndices[node.leftFirst + i]];

                bool hit = intersectTriangle(ray, tri, tmp);

                if (hit && tmp.dist > 0) {
                    if (tmp.dist < closest) {
                        anyHit = true;
                        closest = tmp.dist;
                        rayhit = tmp;
                    }
                }
            }
        }
    }


    return anyHit;
}

bool intersectObject(Ray ray, Object object, float alreadyClosest, out RayHit rayhit) {
    float closest = alreadyClosest;

    mat4 transform = transforms[object.transformID];
    mat4 inverseTransform = inverse(transform);

    Mesh mesh = meshes[object.meshID];
    uint objectBaseIndex = mesh.firstTriangleIndex;

    // Transform ray into object space
    Ray objSpaceRay = Ray((inverseTransform * vec4(ray.origin, 1)).xyz, (inverseTransform * vec4(ray.direction, 0)).xyz);

    // Save length of transformed direction before normalizing, need it to transform t value back to world distances
    float lengthScaling = length(objSpaceRay.direction);
    objSpaceRay.direction = normalize(objSpaceRay.direction);

    const bool doBVH = false;

    if (doBVH) {
        // TODO I think this is correct ??
        closest *= lengthScaling;

        bool hit = intersectBVH(objSpaceRay, mesh.bvhRootIndex, closest, rayhit);

        if (!hit) return false;

        rayhit.dist /= lengthScaling;

        vec4 tmp = transform * vec4(rayhit.position, 1);
        rayhit.position = tmp.xyz / tmp.w;
        mat4 normalMat = transpose(inverseTransform);
        rayhit.normal = normalize((normalMat * vec4(rayhit.normal, 0)).xyz);

        return true;
    } else {
        AABBHitRes hitRes = intersectAABB(objSpaceRay, mesh.aabb);
        float dist = hitRes.inside ? hitRes.t_max : hitRes.t_min;
        dist /= lengthScaling;

        if (!hitRes.hit || dist > closest) {
            return false;
        }

        bool anyHit = false;

        RayHit tmp;
        for (int i = 0; i < mesh.triangleCount; i++) {
            Triangle triangle = triangles[objectBaseIndex + i];

            bool hit = intersectTriangle(objSpaceRay, triangle, tmp);

            if (hit && tmp.dist > 0) {
                tmp.dist /= lengthScaling;
                if (tmp.dist < closest) {
                    anyHit = true;
                    closest = tmp.dist;
                    rayhit = tmp;
                }
            }
        }

        if (anyHit) {
            vec4 tmp = transform * vec4(rayhit.position, 1);
            rayhit.position = tmp.xyz / tmp.w;
            mat4 normalMat = transpose(inverseTransform);
            rayhit.normal = normalize((normalMat * vec4(rayhit.normal, 0)).xyz);
        }

        return anyHit;
    }
}

bool intersectObject(Ray ray, Object object, out RayHit rayhit) {
    return intersectObject(ray, object, 10000000, rayhit);
}