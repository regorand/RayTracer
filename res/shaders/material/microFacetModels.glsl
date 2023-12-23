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

#include "res/shaders/material/material.glsl"
#include "res/shaders/utils/rng.glsl"
#include "res/shaders/utils/Constants.glsl"

float GGXNormalDistFunc(float alpha, vec3 m, vec3 n, float thetaM) {
    float alpha_2 = alpha * alpha;

    float xi = max(dot(n, m), 0.0f);
    float cosThetaM = cos(thetaM);
    float tanThetaM = tan(thetaM);

    return alpha_2 * xi / (PI * pow(cosThetaM, 4.0f) * thetaM * pow(alpha_2 + pow(tanThetaM, 2.0f), 2.0f));
}

float shadowingFunction(float alpha, vec3 m, vec3 n, vec3 i, float thetaV) {
    float alpha_2 = alpha * alpha;

    float vDotM = dot(i, m);
    float vDotN = dot(i, n);

    float xi = max(vDotM / vDotN, 0.0f);

    float tanThetaV = tan(thetaV);

    return xi * 2 / (1 + sqrt(1.0f + alpha_2 * pow(tanThetaV, 2.0f)));
}

vec3 schlickFresnel(vec3 F0, vec3 n, vec3 i) {
    //
    float r = oldIOR / newIOR;
    float c = dot(-n, i);

    return F0 + (1 - F0) * pow(1.0f - c, 5.0f);
}


// returns sampled micronormal in tangent space
vec3 sampleGGX(float roughness, float seed, out float thetaM) {
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

vec3 evaluateBRDF(vec3 i, vec3 n, out vec3 o, out float pdf, float seed, MaterialTMP material) {

    // STEP 1. sample microsurface normal m
    float thetaM = 0;
    vec3 tangentM = sampleGGX(material.roughness, seed, thetaM);

    // only do isotropic material models for now, the tangent space doesn't matter
    vec3 tangent = vec3(n.y, n.z, -n.x);
    vec3 cotangent = cross(n, tangent);

    // transform sampled micronormal into world space
    vec3 m = mat3(tangent, cotangent, n) * tangentM;

    // STEP 2. compute outgoing direction o -> need to give this to caller -> out function parameter
    o = reflect(i, m); // reflection along microsurface normal

    // STEP 3. compute Fresnel, NDF, G

    vec3 F0 = mix(vec3(0.04f), material.albedo, material.metallic);
    vec3 fresnel = schlickFresnel(F0, n, i);

    float NDF = GGXNormalDistFunc(material.roughness, m, n, thetaM);

    float thetaV = acos(dot(n, -i));
    float G = shadowingFunction(material.roughness, m, n, i, thetaV);


    // STEP 4. compute pdf

    float reflectionPDF = 4 * dot(i, n) * dot(o, n);

    pdf = reflectionPDF;
    // STEP 5. compute brdf
    return fresnel * NDF * G / reflectionPDF;
}

