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

    float tanThetaM = max(1e-3, tan(thetaM));

    return alpha_2 * xi / (PI * pow(cosThetaM, 4.0f) * pow(alpha_2 + pow(tanThetaM, 2.0f), 2.0f));
    // NDF in desmos f\left(x\right)=\{t\cdot t\cdot\max\left(\cos\left(x\right),\ 0\right)}{\pi\left(\cos\left(x\right)\right)^{4}\left(t^{2}+\tan^{2}\left(x\right)\right)^{2}}\left\{-\frac{\pi}{2}\le x\le\ \frac{\pi}{2}\right\}
}

float shadowingFunction(float alpha, vec3 m, vec3 n, vec3 i, float thetaV) {
    float alpha_2 = alpha * alpha;

    float vDotM = dot(i, m);
    float vDotN = dot(i, n);

    float xi = max(vDotM / vDotN, 0.0f);

    float tanThetaV = max(1e-3, tan(thetaV));

    return xi * 2 / (1 + sqrt(1.0f + alpha_2 * pow(tanThetaV, 2.0f)));
}

vec3 schlickFresnel(vec3 F0, vec3 n, vec3 i) {
    // float r = oldIOR / newIOR;
    float c = dot(-n, i);

    return F0 + (1 - F0) * pow(1.0f - c, 5.0f);
}


// returns sampled micronormal in tangent space
vec3 sampleGGX(float roughness, inout uint seed, out float thetaM) {
    float eta1 = randUniform_0_1(seed);
    float eta2 = randUniform_0_1(seed);

    float eps = 1e-4;
    thetaM = atan(roughness * sqrt(eta1), sqrt(1.0f - eta1) + 1e-5);
    float phi_m = 2 * PI * eta2;

    float sin_theta_m = sin(thetaM);
    float cos_theta_m = cos(thetaM);
    float sin_phi_m = sin(phi_m);
    float cos_phi_m = cos(phi_m);

    return vec3(sin_theta_m * cos_phi_m, cos_theta_m, sin_theta_m * sin_phi_m);
}

void sampleMicronormal(vec3 N, float roughness, inout uint seed, out vec3 m, out float thetaM, out float NDF, out float pdf) {
    vec3 tangentM = sampleGGX(roughness, seed, thetaM);

    // only do isotropic material models for now, the tangent space doesn't matter
    vec3 tangent = vec3(N.y, N.z, -N.x);
    vec3 cotangent = cross(N, tangent);

    // transform sampled micronormal into world space
    m = normalize(mat3(tangent, N, cotangent) * tangentM);

    NDF = GGXNormalDistFunc(roughness, m, N, thetaM);
    pdf = NDF * dot(m, N);
}

void evaluateBRDF(vec3 inDir, vec3 N, vec3 m, float thetaM, float NDF, vec3 outDir, Material material, out vec3 brdf) {
    vec3 albedo = vec3(1);
    if (material.metallic < 1.0 /* TODO && material.transmission < 1.0 */) {
        albedo = material.albedo.xyz; // diffuse term here
    }

    vec3 F0 = mix(vec3(0.04f), material.albedo.xyz, material.metallic);

    vec3 fresnel = schlickFresnel(F0, m, inDir);

    // float NDF = GGXNormalDistFunc(material.roughness, m, N, thetaM);

    float thetaV = acos(dot(N, -inDir));
    float G = shadowingFunction(material.roughness, m, N, inDir, thetaV);

    // STEP 5. compute brdf
    float demon = 4 * abs(dot(inDir, N)) * abs(dot(outDir, N));

    brdf = albedo * fresnel * NDF * G / demon;
}