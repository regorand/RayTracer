#include "res/shaders/material/LambertDiffuse.glsl"

Sample CoatedDiffuseSample(vec3 w_o, Material mat, inout uint seed) {
    const float specular = (mat.specular.x + mat.specular.y + mat.specular.z) / 3;

    float R = clamp(specular * fresnelEquation(w_o.z, 1, mat.IOR), 0, 1);
    if (randUniform_0_1(seed) <= R) {
        return GGXSample(w_o, mat, vec3(R), seed);
    }

    return LambertDiffuseSample(w_o, mat, seed);
}

vec3 CoatedDiffuseEval(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    const float specular = (mat.specular.x + mat.specular.y + mat.specular.z) / 3;
    float R = clamp(specular * fresnelEquation(w_o.z, 1, mat.IOR), 0, 1);

    return R * GGXEval(w_o, w_i, mat, vec3(R), seed) + (1 - R) * LambertDiffuseEval(w_o, w_i, mat, seed);
}

float CoatedDiffusePDF(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    float R = fresnelEquation(w_o.z, mat.IOR, 1);

    float rand = randUniform_0_1(seed);
    if (rand <= R) {
        return GGXPDF(w_o, w_i, mat, seed);
        // return 0;
    }

    return LambertDiffusePDF(w_o, w_i, mat, seed);
}