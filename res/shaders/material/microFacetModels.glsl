/*
TODO:
- Implement some NDF and G
- Implement respective importance sampling, pdf sampling
- Implement general microfacet model evaluation
- Implement reflective and refractive microsurface BxDFs and combine to macro BSDF
*/

/*
Calculation Steps:

- Receive incoming direction i, geometric normal n
- Sample microsurface normal m
- compute outgoing direction o
- compute Fresnel, NDF, G for m
- compute pdf (which measure space ? -> in o ?)
- compute vec3 result -> outgoing radiance

Question -> Where do I modulate albedo into specular reflection for metallic materials ?
*/

struct MaterialTMP {
    vec3 albedo;
    float roughness;
    float metallic;
    // transmittance factor ?

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material
    // sample metallic value to decide if to evalutate metallic or dielectric bsdf ?
    // -> need to consider this in pdf -> is that just a constant factor ?
    // need sheen layer for colorless highlights ?
    // metallic seems to have higher prio than transmittance of dielectric
    // -> look at principled bsdf shader example!
}

void GGXNormalDistFunc(float alpha) {

}

#include "res/shaders/utils/rng.glsl"
#include "res/shaders/utils/Constants.glsl"

// returns sampled micronormal in tangent space
vec3 sampleGGX(float roughness, float seed) {
    float eta1 = randUniform_0_1(seed);
    float eta2 = randUniform_0_1(seed);

    float eps = 1e-4;
    float theta_m = atan(roughness * sqrt(eta1), sqrt(1.0f - eta1) + eps);
    float phi_m = 2 * PI * eta2;

    float sin_theta_m = sin(theta_m);
    float cos_theta_m = cos(theta_m);
    float sin_phi_m = sin(phi_m);
    float cos_phi_m = cos(phi_m);

    return vec3(sin_theta_m * cos_phi_m, cos_theta_m, sin_theta_m * sin_phi_m);
}

vec3 evaluateBRDF(vec3 i, vec3 n, out vec3 o, float seed, MaterialTMP material) {

    // STEP 1. sample microsurface normal m
    vec3 tangentM = sampleGGX(material.roughness, seed);

    // only do isotropic material models for now, the tangent space doesn't matter
    vec3 tangent = vec3(n.y, n.z, -n.x);
    vec3 cotangent = cross(n, tangent);

    // transform sampled micronormal into world space
    vec3 m = mat3(tangent, cotangent, n) * tangentM;

    // STEP 2. compute outgoing direction o -> need to give this to caller -> out function parameter
    o = reflect(i, m); // reflection along microsurface normal

    // STEP 3. compute Fresnel, NDF, G


    // STEP 4. compute pdf
    // STEP 5. compute radiance
}

