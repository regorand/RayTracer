#include "res/shaders/utils/Constants.glsl"
#include "res/shaders/utils/rng.glsl"

float GGX_NDF(vec3 m, float a_x, float a_y) {
    float d1 = m.x / a_x;
    float d2 = m.y / a_y;
    float sdenom = d1 * d1 + d2 * d2 + m.z * m.z;

    return 1 / (PI * a_x * a_y * sdenom * sdenom);
}

float Smith_Shadowing(vec3 V, float a_x, float a_y) {
    float discr = 1 + (a_x * a_x * V.x * V.x + a_y * a_y * V.y * V.y) / (V.z * V.z);
    float lambda = (-1 + sqrt(discr)) / 2;
    return 1 / (1 + lambda);
}

vec3 schlickFresnel(vec3 i, vec3 o, vec3 F0) {
    vec3 m = normalize((i + o) / 2);
    float cos_theta = max(dot(i, m), 0.0f);

    return F0 + (1 - F0) * pow(1 - cos_theta, 5.0f);
}

/*
 * Implementation taken and adapted from:
    Heitz, E. (2018). Sampling the ggx distribution of visible normals. Journal of Computer Graphics Techniques (JCGT), 7(4), 1-13.

 */
vec3 sampleGGXVNDF(vec3 Ve, float a_x, float a_y, inout uint seed) {
    // 1. Transform view dir into hemisphere configuration
    vec3 Vh = normalize(vec3(a_x * Ve.x, a_y * Ve.y, Ve.z));

    // 2. Create Orthonormal basis
    float lensq = Vh.x * Vh.x + Vh.y * Vh.y;
    vec3 T1 = lensq > 0 ? vec3(-Vh.y, Vh.x, 0) * inversesqrt(lensq) : vec3(1,0,0);
    vec3 T2 = cross(Vh, T1);

    // 3. Parameterization of projected area
    float U1 = randUniform_0_1(seed);
    float U2 = randUniform_0_1(seed);
    float r = sqrt(U1);
    float phi = 2.0 * PI * U2;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + Vh.z);
    t2 = (1.0 - s)*sqrt(1.0 - t1*t1) + s*t2;

    // 4. Reprojection onto hemisphere
    vec3 Nh = t1*T1 + t2*T2 + sqrt(max(0.0, 1.0 - t1*t1 - t2*t2))*Vh;

    // 5. Transforming normal back to ellipsoid
    vec3 Ne = normalize(vec3(a_x * Nh.x, a_y * Nh.y, max(0.0, Nh.z)));
    return Ne;
}

Sample GGXSample(vec3 w_o, vec3 N, Material mat, inout uint seed) {
    Sample smpl;
    if (w_o.z <= 0.0f) {
        smpl.brdf = vec3(0);
        smpl.newDir = vec3(0, 0, 1); // Don't return an invalid direction here
        smpl.pdf = 1.0f;
        return smpl;
    }
    vec3 m = sampleGGXVNDF(w_o, mat.roughness, mat.roughness, seed);

    vec3 w_i = reflect(-w_o, m);

    float G1_o = Smith_Shadowing(w_o, mat.roughness, mat.roughness);
    float G1_i = Smith_Shadowing(w_i, mat.roughness, mat.roughness);
    float G2 = G1_o * G1_i;

    vec3 F0 = mat.albedo.xyz; //mix(vec3(0.04f), mat.albedo.xyz, mat.metallic);
    vec3 F = schlickFresnel(w_i, w_o, F0);

    // Don't need to eval. everything here
    // Using this with importance sampling in a MC estimator we can directly divide out the pdf from the contribution
    // Yielding a simpler expression
    // float D = GGX_NDF(m, mat.roughness, mat.roughness);
    // smpl.brdf = F * D * G2 / denom;
    // float dot_m_w_o = dot(m, w_o);
    // smpl.pdf = (G1_o * max(dot_m_w_o, 0.0f) * D / w_o.z) / (4 * dot_m_w_o);

    smpl.newDir = w_i;
    smpl.brdf = F * G2 / G1_o;
    smpl.pdf = 1.0f;
    return smpl;
}

vec3 GGXEval(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    if (w_i.z <= 0.0f || w_o.z <= 0) {
        return vec3(0.0f);
    }

    vec3 m = normalize((w_i + w_o) / 2);
    float D = GGX_NDF(m, mat.roughness, mat.roughness);

    float G1_o = Smith_Shadowing(w_o, mat.roughness, mat.roughness);
    float G1_i = Smith_Shadowing(w_i, mat.roughness, mat.roughness);
    float G2 = G1_o * G1_i;

    vec3 F0 = mat.albedo.xyz; //mix(vec3(0.04f), mat.albedo.xyz, mat.metallic);
    vec3 F = schlickFresnel(w_i, w_o, F0);

    float denom = 4.0f * w_i.z * w_o.z;

    return F * D * G2 / denom;
}

float GGXPDF(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    if (w_i.z <= 0.0f || w_o.z <= 0.0f) {
        return 1.0f; // Don't return 0.0f here to prevent Nans and infinities
    }
    vec3 m = normalize((w_i + w_o) / 2);
    float D = GGX_NDF(m, mat.roughness, mat.roughness);

    float G1_o = Smith_Shadowing(w_o, mat.roughness, mat.roughness);

    float dot_m_w_o = dot(m, w_o);
    return (G1_o * max(dot_m_w_o, 0.0f) * D / w_o.z) / (4 * dot_m_w_o);
}