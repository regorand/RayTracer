#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/utils/samplingUtils.glsl"
#include "res/shaders/material/microFacetModels.glsl"

#include "res/shaders/lighting/NEE.glsl"

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
void evaluateLighting(Ray inRay, RayHit rayHit, inout vec3 mul, inout uint seed, out Ray outRay, out bool hasDirect, out vec3 outDirect) {
    const float eps = 1e-4;

    vec3 N = rayHit.hitBackface ? -rayHit.normal : rayHit.normal;
    Ray newRay;
    outRay.origin = rayHit.position;

    // TODO direct lighting
    hasDirect = false;
    outDirect = vec3(0);

    const bool doDirect = true;
    if (doDirect && rayHit.mat.emission > 0.0f) {
        const vec3 LIGHT_INTENSITY = vec3(15);

        float emissionLambert = max(0.0f, dot(N, -inRay.direction));

        outDirect = emissionLambert * mul * LIGHT_INTENSITY;
        hasDirect = true;
        mul = vec3(0);
    } else /* Diffuse */ {
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

        /*
        // TODO NEE
        vec3 contribNEE;
        float pdfNEE;
        sampleLightsourceNEE(rayHit.position, N, inRay.direction, rayHit.mat, seed, contribNEE, pdfNEE);

        outDirect += contribNEE;
        hasDirect = true;
*/
    }

    outRay.origin += outRay.direction * eps;
}