// Needs to be before inputs of different BSDF Implementations
struct Sample {
    vec3 brdf;
    vec3 newDir;
    float pdf;
};


#include "res/shaders/material/LambertDiffuse.glsl"
#include "res/shaders/material/GGX.glsl"

mat3 getArbitraryTangentSpace(vec3 N) {
    vec3 tangent = vec3(N.y, N.z, -N.x);
    vec3 cotangent = cross(N, tangent);

    // transform sampled micronormal into world space
    return mat3(tangent, cotangent, N);
}

Sample BSDF_Sample(vec3 w_o, vec3 N, Material mat, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = transpose(tangentFrame);

    vec3 wt_o = invFrame * -w_o;

    Sample smpl;
    if (useMicrofacets) {
        smpl = GGXSample(wt_o, N, mat, seed);
    } else {
        smpl = LambertDiffuseSample(wt_o, N, mat, seed);
    }

    smpl.newDir = tangentFrame * smpl.newDir;
    return smpl;
}

vec3 BSDF_Eval(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = transpose(tangentFrame);

    // in and out dir in
    vec3 wt_o = invFrame * -w_o;
    vec3 wt_i = invFrame *  w_i;

    if (useMicrofacets) {
        return GGXEval(wt_o, wt_i, N, mat, seed);
    } else {
        return LambertDiffuseEval(wt_o, wt_i, N, mat, seed);
    }
}

/*
 * w_i: Direction the ray is going to keep being traced
 * w_o: Direction the ray is coming from
 * N:   World Space macro normal of the surface hit
 */
float BSDF_PDF(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = inverse(tangentFrame);

    // in and out dir in
    vec3 wt_o = invFrame * -w_o;
    vec3 wt_i = invFrame *  w_i;
    if (useMicrofacets) {
        return GGXPDF(wt_o, wt_i, N, mat, seed);
    } else {
        return LambertDiffusePDF(wt_o, wt_i, N, mat, seed);
    }
}

