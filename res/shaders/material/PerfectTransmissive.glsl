#include "res/shaders/material/materialUtils.glsl"

Sample PerfectTransmissive_Sample(vec3 w_o, vec3 N, Material mat, bool exiting, inout uint seed) {
    float c = dot(w_o, -N);
    float R = 0;
    float r = 1 / mat.IOR;

    if (exiting) {
        R = fresnelEquation(c, mat.IOR, 1);
        r = mat.IOR;
    } else {
        R = fresnelEquation(c, 1, mat.IOR);
    }

    Sample smpl;

    float k = 1.0f - r * r * (1 - c * c);

    if (k < 0 || randUniform_0_1(seed) <= R) {
        // Reflect
        smpl.newDir = reflect(w_o, N);
        // smpl.brdf = vec3(0, 0, 1);
        smpl.brdf = vec3(1) / (max(0.0f, dot(smpl.newDir, N)) + EPS_4);
        smpl.pdf = k < 0 ? 1.0f : 1.0f / R;
        return smpl;
    }

    smpl.newDir = normalize((r * c - sqrt(k)) * N - r * -w_o);
    // smpl.brdf = vec3(1, 0, 0); // mat.albedo.xyz;
    smpl.brdf = mat.albedo.xyz / (dot(smpl.newDir, N) + EPS_4);
    smpl.pdf = 1.0f / (1.0f - R);

    return smpl;
}

// Can never hit perfect transmission direction exactly, so when not directly sampled it evaluates to 0
vec3 PerfectTransmissive_Eval(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    return vec3(0);
}

float PerfectTransmissive_PDF(vec3 w_o, vec3 w_i, Material mat, inout uint seed) {
    return 0;
}
