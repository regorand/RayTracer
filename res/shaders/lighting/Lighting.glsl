#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/utils/samplingUtils.glsl"
#include "res/shaders/material/microFacetModels.glsl"

#include "res/shaders/lighting/NEE.glsl"

struct LightingSettings {
    bool evaluateDirect;
    bool evaluateNEE;
};

/*  *
    * Evaluates lighting and samples the next ray.
    * Params:
    *   inRay: incoming ray which caused the intersection
    *   rayHit: RayHit struct describing the hit
    *   mul: accumulated color multiplier of the entire path: Used to compute direct light contribution. This function
            will write the new multiplier into this inout variable
    *   seed: inout seed which will be used to generate all pseudo random numbers used e.g. for sampling
    *   outRay: out variable which specifies the new ray along the newly sampled direction
    *   hasDirect: boolean output variable specifying, whether there is a direct light contribution from directly
            hitting an emissive object.
    *   outDirect: Direct light contribution
    */
void evaluateLighting(Ray inRay, RayHit rayHit, LightingSettings settings, inout vec3 mul, inout uint seed, out Ray outRay, out bool hasDirect, out vec3 outDirect) {
    float eta = randUniform_0_1(seed);

    vec3 N = rayHit.hitBackface ? -rayHit.normal : rayHit.normal;
    Ray newRay;
    outRay.origin = rayHit.position;

    hasDirect = false;
    outDirect = vec3(0);

    if (settings.evaluateDirect && rayHit.mat.emission > 0.0f) {
        const vec3 LIGHT_INTENSITY = rayHit.mat.emissionColor.xyz;

        float emissionLambert = 1;// max(0.0f, dot(N, -inRay.direction));

        outDirect += emissionLambert * mul * LIGHT_INTENSITY;
        hasDirect = true;
        mul = vec3(0);
    }

    float pdf = 1;
    vec3 m = vec3(0);
    float thetaM = 0;
    float NDF = 0;
    sampleMicronormal(N, rayHit.mat.roughness, seed, m, thetaM, NDF, pdf);


    outRay.direction = reflect(inRay.direction, m);
    vec3 brdf = vec3(0);
    evaluateBRDF(inRay.direction, N, m, thetaM, NDF, outRay.direction, rayHit.mat, brdf);

    float newDirLambert = max(0.0f, dot(N, outRay.direction));
    mul *= newDirLambert * brdf / pdf;

    if (settings.evaluateNEE) {
        vec3 contribNEE = vec3(0);
        float pdfNEE = 1;
        sampleLightsourceNEE(rayHit.position, N, inRay.direction, rayHit.mat, seed, contribNEE, pdfNEE);

        outDirect += contribNEE / pdfNEE;
        hasDirect = true;


    }

    outRay.origin += outRay.direction * EPS_4;
}