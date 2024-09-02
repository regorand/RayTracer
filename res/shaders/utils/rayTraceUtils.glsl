#include "res/shaders/utils/intersectUtils.glsl"
#include "src/deviceShared/BufferBindings.h"
#include "src/deviceShared/MaterialFlags.h"
#include "res/shaders/utils/FlagUtils.glsl"

bool getClosestHit(Ray ray, out RayHit rayhit) {
    bool anyHit = false;
    float closest = 10000000;

    RayHit tmp;
    for (int i = 0; i < objectCount; i++) {
        Mesh mesh = meshes[i];

        bool hit = intersectObject(ray, mesh, closest, tmp);

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
        Mesh mesh = meshes[i];

        // reduce maxDist by eps value, so that if maxDist is given by a known position on geometry,
        // that geometry does not shadow itself
        RayHit dummy;

        if (intersectObject(ray, mesh, maxDist - 1e-3, dummy)) {
            return true;
        }
    }
    return false;
}

/*
vec3 getTransmittedDir(float oldIOR, float newIOR, vec3 N, vec3 dir) {
    float R_0 = pow((oldIOR - newIOR) / (oldIOR + newIOR), 2.0f);

    float r = oldIOR / newIOR;
    float c = dot(-N, dir);

    float fresnel = R_0 + (1 - R_0) * pow(1.0f - c, 5.0f); // Schlick's Approximation for the Fresnel effect;

    if (rand(frameSeed) < fresnel) {
        return reflect(dir, N);
    }

    // refraction calculation from https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal#:~:text=Let%20n%20be%20the%20normalized,c%20%3D%20%2Dn%20dot%20V_incedence.

    float cos_theta_2_sqr = 1.0f - pow(r, 2.0f) * (1.0f - c * c);
    if (cos_theta_2_sqr < 0) {
        return reflect(dir, N);
    } else {
        return normalize(r * dir + N * (r * c - sqrt(cos_theta_2_sqr)));
    }
}
*/

/*
vec3 sampleNEETarget(vec2 seed, out Triangle triangle) {
    // terrible name, but it indexes into the array of indices
    int sampledIndexIndex = int(max(rand(seed) * LIGHT_SOURCE_COUNT - 1e-5, 0.0f)); // complicated to make 100% sure index can not be the same as LIGHT_SOURCE_COUNT

    int primitiveIndex = LIHGT_INDICES[sampledIndexIndex];

    triangle = triangles[primitiveIndex];
    return sampleTrianglePos(triangle, seed);
}
*/