#include "res/shaders/utils/Constants.glsl"
#include "res/shaders/utils/samplingUtils.glsl"

Sample LambertDiffuseSample(vec3 w_o, Material mat, inout uint seed) {
    Sample smpl;
    smpl.newDir = sampleHemisphereCosine(seed, smpl.pdf);

    smpl.brdf = mat.albedo.xyz * ONE_OVER_PI;
    return smpl;
}

vec3 LambertDiffuseEval(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    return mat.albedo.xyz * ONE_OVER_PI;
}

float LambertDiffusePDF(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    return w_i.z * ONE_OVER_PI;
}