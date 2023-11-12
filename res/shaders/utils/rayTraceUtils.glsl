#include "res/shaders/utils/intersectUtils.glsl"
#include "src/deviceShared/BufferBindings.h"
#include "src/deviceShared/MaterialFlags.h"
#include "res/shaders/utils/FlagUtils.glsl"

bool getClosestHit(Ray ray, out RayHit rayhit) {
    bool anyHit = false;
    float closest = 1000000;

    RayHit tmp;
    for (int i = 0; i < objectCount; i++) {
        MeshObject meshObject = meshObjects[i];
        bool hit = intersectObject(ray, meshObject, tmp);
        if (hit && tmp.dist < closest) {
            anyHit = true;
            closest = tmp.dist;
            rayhit = tmp;
            if (meshObject.emissive == 1) {
                uint props = EMISSIVE_FLAG;
                rayhit.mat.color.w = uintBitsToFloat(props);
            }
        }
    }

    return anyHit;
}

/*
vec3 sampleNEETarget(vec2 seed, out Triangle triangle) {
    // terrible name, but it indexes into the array of indices
    int sampledIndexIndex = int(max(rand(seed) * LIGHT_SOURCE_COUNT - 1e-5, 0.0f)); // complicated to make 100% sure index can not be the same as LIGHT_SOURCE_COUNT

    int primitiveIndex = LIHGT_INDICES[sampledIndexIndex];

    triangle = triangles[primitiveIndex];
    return sampleTrianglePos(triangle, seed);
}
*/