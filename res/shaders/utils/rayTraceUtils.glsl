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

        // TODO rewrite intersect functions to accept max distance (here pass in closest), so we can early exit
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

vec3 getTransmittedDir(float oldIOR, float newIOR, vec3 N, vec3 dir) {
    // refraction calculation from https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal#:~:text=Let%20n%20be%20the%20normalized,c%20%3D%20%2Dn%20dot%20V_incedence.
    float r = oldIOR / newIOR;
    float c = dot(-N, dir);

    vec3 newDir = vec3(0);
    float cos_theta_2_sqr = 1.0f - pow(r, 2.0f) * (1.0f - c * c);
    if (cos_theta_2_sqr < 0) {
        return reflect(dir, N);
    } else {
        return r * dir + N * (r * c - sqrt(cos_theta_2_sqr));
    }
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