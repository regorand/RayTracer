// Needs to be before inputs of different BSDF Implementations
struct Sample {
    vec3 brdf;
    vec3 newDir;
    float pdf;
};


#include "res/shaders/material/LambertDiffuse.glsl"
#include "res/shaders/material/GGX.glsl"
#include "res/shaders/material/PerfectTransmissive.glsl"

/*
TODO "Principled" BSDF
Want:
- Glossy Metallic
- Glossy Diffuse
- Non-Glossy Diffuse
- Transmissive
    -> And Rough Transmissive ?
- Emissive

Material Properties:
- Albedo (diffuse color, transmission color, metalltic F0)
- Roughness (For glossy metallic, glossy diffuse, (rough transmission))
- Emission color
- metallic (blend factor for diffuse -> metallic material)
- transmissive (blend factor for diffuse -> transmissive)
- specular (strength of specular coat of diffuse material, at 1 means full F0 = 0.04 ?)
- IOR

- If I really want: at some point normal incident reflection


Decision Tree for Material
For now: no Interpolation between material types, I don't know how I would importance sample
    -> Might work with multiple importance sampling and then sample one sampling strategy accordingly ?
- Emissive is not used for surface reflection

1. Transmissive -> sample if transmitted:   If yes: Fresnel formula, reflection or transmission, DONE
                                                TODO: rough tranmission from paper
                                            If no: continue
2. Metallic -> if > 0.5 -> Glossy Metallic, DONE
3. Diffuse, specular controls Fresnel Blend Factor

*/


mat3 getArbitraryTangentSpace(vec3 N) {
    vec3 tangent = vec3(N.y, N.z, -N.x);
    vec3 cotangent = cross(N, tangent);

    // transform sampled micronormal into world space
    return mat3(tangent, cotangent, N);
}

Sample BSDF_Sample(vec3 w_o, vec3 N, Material mat, bool exiting, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = transpose(tangentFrame);

    vec3 wt_o = -(invFrame * w_o);

    Sample smpl;

    float rand = randUniform_0_1(seed);
    if (rand <= mat.transparency) {
        return PerfectTransmissive_Sample(w_o, N, mat, exiting, seed);
    }
    rand = randUniform_0_1(seed);
    if (rand <= mat.metallic) {
        smpl = GGXSample(wt_o, mat, seed);
        smpl.newDir = tangentFrame * smpl.newDir;
        return smpl;
    }
    smpl = GGXSample(wt_o, mat, seed);

    smpl.newDir = tangentFrame * smpl.newDir;
    return smpl;
}

vec3 BSDF_Eval(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = transpose(tangentFrame);

    // in and out dir in
    vec3 wt_o = invFrame * -w_o;
    vec3 wt_i = invFrame *  w_i;

    /*
    float rand = randUniform_0_1(seed);
    if (rand <= mat.transparency) {
        return PerfectTransmissive_Eval(w_o, w_i, mat, seed);
    }
    rand = randUniform_0_1(seed);

    if (rand <= mat.metallic) {
        return GGXEval(wt_o, wt_i, mat, seed);
    }
    */

    return LambertDiffuseEval(wt_o, wt_i, mat, seed);
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

    float rand = randUniform_0_1(seed);
    if (rand <= mat.transparency) {
        return PerfectTransmissive_PDF(w_o, w_i, mat, seed);
    }
    rand = randUniform_0_1(seed);

    if (rand <= mat.metallic) {
        return GGXPDF(wt_o, wt_i, mat, seed);
    }

    return LambertDiffusePDF(wt_o, wt_i, mat, seed);
}

/*
// Needs to be before inputs of different BSDF Implementations
struct Sample {
    vec3 brdf;
    vec3 newDir;
    float pdf;
};


#include "res/shaders/material/LambertDiffuse.glsl"
#include "res/shaders/material/GGX.glsl"
#include "res/shaders/material/PerfectTransmissive.glsl"

/*
TODO "Principled" BSDF
Want:
- Glossy Metallic
- Glossy Diffuse
- Non-Glossy Diffuse
- Transmissive
    -> And Rough Transmissive ?
- Emissive

Material Properties:
- Albedo (diffuse color, transmission color, metalltic F0)
- Roughness (For glossy metallic, glossy diffuse, (rough transmission))
- Emission color
- metallic (blend factor for diffuse -> metallic material)
- transmissive (blend factor for diffuse -> transmissive)
- specular (strength of specular coat of diffuse material, at 1 means full F0 = 0.04 ?)
- IOR

- If I really want: at some point normal incident reflection


Decision Tree for Material
For now: no Interpolation between material types, I don't know how I would importance sample
    -> Might work with multiple importance sampling and then sample one sampling strategy accordingly ?
- Emissive is not used for surface reflection

1. Transmissive -> sample if transmitted:   If yes: Fresnel formula, reflection or transmission, DONE
                                                TODO: rough tranmission from paper
                                            If no: continue
2. Metallic -> if > 0.5 -> Glossy Metallic, DONE
3. Diffuse, specular controls Fresnel Blend Factor

mat3 getArbitraryTangentSpace(vec3 N) {
    vec3 tangent = vec3(N.y, N.z, -N.x);
    vec3 cotangent = cross(N, tangent);

    // transform sampled micronormal into world space
    return mat3(tangent, cotangent, N);
}

Sample BSDF_Sample(vec3 w_o, vec3 N, Material mat, bool exiting, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = transpose(tangentFrame);

    vec3 wt_o = -(invFrame * w_o);

    Sample smpl;

    if (mat.transparency >= 0.99) {
        return PerfectTransmissive_Sample(w_o, N, mat, exiting, seed);
    } else {
        if (useMicrofacets) {
            smpl = GGXSample(wt_o, mat, seed);
        } else {
            smpl = LambertDiffuseSample(wt_o, mat, seed);
        }
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
    if (mat.transparency >= 0.99) {
        return PerfectTransmissive_Eval(w_o, w_i, mat, seed);
    } else {
        if (useMicrofacets) {
            return GGXEval(wt_o, wt_i, mat, seed);
        } else {
            return LambertDiffuseEval(wt_o, wt_i, mat, seed);
        }
    }
}

/*
 * w_i: Direction the ray is going to keep being traced
 * w_o: Direction the ray is coming from
 * N:   World Space macro normal of the surface hit

float BSDF_PDF(vec3 w_o, vec3 w_i, vec3 N, Material mat, inout uint seed) {
    mat3 tangentFrame = getArbitraryTangentSpace(N);
    mat3 invFrame = inverse(tangentFrame);

    // in and out dir in
    vec3 wt_o = invFrame * -w_o;
    vec3 wt_i = invFrame *  w_i;
    if (useMicrofacets) {
        return GGXPDF(wt_o, wt_i, mat, seed);
    } else {
        return LambertDiffusePDF(wt_o, wt_i, mat, seed);
    }
}


*/