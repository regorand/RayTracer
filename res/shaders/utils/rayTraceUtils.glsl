#include "res/shaders/utils/intersectUtils.glsl"
#include "src/deviceShared/BufferBindings.h"
#include "src/deviceShared/MaterialFlags.h"
#include "res/shaders/utils/FlagUtils.glsl"
#include "res/shaders/scenes/CPUScene.glsl"
#include "res/shaders/utils/Stack.glsl"

bool getClosestHit(Ray ray, out RayHit rayhit) {
    bool anyHit = false;
    float closest = 10000000;

    RayHit tmp;
    for (int i = 0; i < objectCount; i++) {
        Object object = objects[i];

        bool hit = intersectObject(ray, object, closest, tmp);

        if (hit && tmp.dist < closest) {
            anyHit = true;
            closest = tmp.dist;
            rayhit = tmp;
        }
    }

    return anyHit;
}

bool anyHit(Ray ray, float maxDist) {
    for (int i = 0; i < objectCount; i++) {
        Object object = objects[i];

        // reduce maxDist by eps value, so that if maxDist is given by a known position on geometry,
        // that geometry does not shadow itself
        RayHit dummy;

        if (intersectObject(ray, object, maxDist - 1e-3, dummy)) {
            return true;
        }
    }
    return false;
}

int countObjectBVHHits(Ray ray, int objectId) {
    Object object = objects[objectId];

    uint bvhRoot = meshes[object.meshID].bvhRootIndex;

    mat4 transform = transforms[object.transformID];
    mat4 inverseTransform = inverse(transform);

    // note, this doesn't take length scaling effects into account because as of right now it doesn't need to
    Ray objSpaceRay = Ray((inverseTransform * vec4(ray.origin, 1)).xyz, (inverseTransform * vec4(ray.direction, 0)).xyz);

    Stack stack;
    stack.currentSize = 0;

    push(stack, bvhRoot);
    uint count = 0;

    while (stack.currentSize > 0) {
        uint nodeIdx = ptop(stack);

        BVHNode node = bvhNodes[nodeIdx];

        AABB aabb;
        aabb.lower = vec4(node.lower, 1);
        aabb.upper = vec4(node.upper, 1);

        AABBHitRes hitRes = intersectAABB(objSpaceRay, aabb);


        if (hitRes.hit && node.triCount == 0) {
            push(stack, node.leftFirst);
            push(stack, node.leftFirst + 1);
        }

        count += int(hitRes.hit);
    }

    return int(count);
}

int countBVHHits(Ray ray, int objectId) {
    return countObjectBVHHits(ray, objectId);
}

int countBVHHits(Ray ray) {
    int intersections = 0;
    for (int i = 0; i < objectCount; i++) {
        intersections += countObjectBVHHits(ray, i);
    }
    return intersections;
}
