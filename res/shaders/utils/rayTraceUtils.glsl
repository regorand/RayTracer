#include "res/shaders/utils/types.glsl"
#include "res/shaders/utils/intersectUtils.glsl"

bool castShadowRay(vec3 point, vec3 lightPos) {
    Ray ray;
    vec3 unnormToLight = lightPos - point;
    float distToLight = length(unnormToLight);
    ray.direction = unnormToLight / distToLight;
    float eps = 1e-1;
    ray.origin = point + eps * ray.direction;

    for (int i = 0; i < 5; i++) {
        float dist;
        bool hit = intersectSphere(ray, spheres[i], dist);
        if (hit && dist < distToLight) return true;
    }
    return false;
}

vec3 getLightContrib(vec3 pos) {
    float dist = distance(pos, lightPos);
    float shadowFac = castShadowRay(pos, lightPos) ? 0 : 1;
    return shadowFac * lightIntensity / (dist * dist);
}

bool getClosestHit(Ray ray, out Sphere closestSphere, out float dist) {
    float closest = 1000000;
    bool anyHit = false;
    int closestIndex = -1;

    for (int i = 0; i < 5; i++) {
        float dist;
        bool hit = intersectSphere(ray, spheres[i], dist);
        if (hit && dist > 0) {
            anyHit = true;
            if (dist < closest) {
                closest = dist;
                closestIndex = i;
            }
        }
    }
    if (anyHit) {
        closestSphere = spheres[closestIndex];
        dist = closest;
    }
    return anyHit;
}

vec3 raytrace(Ray ray) {
    vec2 seed = frameSeed + gl_GlobalInvocationID.xy;

    Sphere hitSphere;

    float dists[MAX_BOUNCES];
    Sphere hitSpheres[MAX_BOUNCES];
    vec3 normals[MAX_BOUNCES];
    Ray rays[MAX_BOUNCES + 1]; // 1 extra direction to prevent branching
    int lastHit = -1;
    rays[0] = ray;
    for (int i = 0; i < MAX_BOUNCES; i++) {
        bool hit = getClosestHit(rays[i], hitSpheres[i], dists[i]);

        if (!hit) break;
        lastHit = i;

        rays[i+1].origin = rays[i].origin + dists[i] * rays[i].direction;
        normals[i] = normalize(rays[i].origin - hitSpheres[i].center);

        float diffuse = hitSpheres[i].mat.diffuse;

        rays[i+1].direction = diffuse * getNewDirection(seed, normals[i]) + (1 - diffuse) * reflect(rays[i].direction, normals[i]);
    }

    if (lastHit == -1) return vec3(0);

    vec3 cumulutiveLight = hitSpheres[lastHit].mat.diffuse * getLightContrib(rays[lastHit + 1].origin);
    rays[lastHit+1].direction = normalize(lightPos - rays[lastHit+1].origin);

    for (int i = lastHit; i >= 0; i--) {
        vec3 hitPos = rays[i].origin + dists[i] * rays[i].direction;
        vec3 N = normalize(hitPos - hitSpheres[i].center);

        float vertexDist = distance(rays[i].origin, rays[i+1].origin);

        float diffuse = hitSpheres[i].mat.diffuse;

        cumulutiveLight = diffuse * hitSpheres[i].mat.color * cumulutiveLight + (1 - diffuse) * cumulutiveLight;
        // cumulutiveLight = diffuse * dot(N, L) * hitSpheres[i].mat.color * lightContrib +
        //                   (1 - diffuse) * hitSpheres[i].mat.color * cumulutiveLight / vertexDist;

    }

    return cumulutiveLight;
}