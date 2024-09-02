#include "res/shaders/scenes/CPUScene.glsl"
#include "res/shaders/utils/MeshTypes.glsl"
#include "res/shaders/utils/samplingUtils.glsl"

Mesh sampleNEEObject(inout uint seed, inout float pdf) {
    // TODO: this is naive implementation, do some kinf of precompute

    // TODO dont uniformly sample light sources

    uint lightSourceIndex = emissiveIndices[randUniform_uint_0_n(seed, emissiveCount)];
    pdf = 1.0f / float(emissiveCount);

    return meshes[lightSourceIndex];
}

Triangle sampleNEETriangle(Mesh mesh, inout uint seed, inout float pdf) {
    // TODO also here, dont uniformly sample, improve

    uint triangleIndex = mesh.firstTriangleIndex + randUniform_uint_0_n(seed, mesh.triangleCount);
    pdf /= mesh.triangleCount;
    return triangles[triangleIndex];
}

void sampleLightsourceNEE(vec3 pos, vec3 N, vec3 inDir, BufferMaterial material, inout uint seed, out vec3 contrib, out float pdf) {
    if (emissiveCount == 0) {
        contrib = vec3(0);
        pdf = 0;
        return;
    }

    Mesh object = sampleNEEObject(seed, pdf);

    Triangle triangle = sampleNEETriangle(object, seed, pdf);

    vec3 sampledPos = sampleTrianglePos(triangle, seed, pdf);

    vec3 dir = sampledPos - pos;
    float dist = length(dir) + EPS_4;
    dir = dir / vec3(dist);

    vec3 halfway = (dir - inDir) / vec3(2);
    float thetaM = acos(dot(halfway, N));

    vec3 brdf = vec3(1);
    evaluateBRDF(inDir, N, halfway, thetaM, dir, material, brdf);
    if (length(brdf) < 1e-4) {
        // The material will have almost zero contribution for this light source
        // For example, performing NEE on a perfectly specular surface
        contrib = vec3(0);
        pdf = 0;
    }

    bool shadowed = anyHit(Ray(pos, dir), dist);

    if (shadowed) {
        contrib = vec3(0);
        return;
    }

    const float lightDotTerm = max(0.0f, dot(dir, N));
    const float surfaceDotTerm = max(0.0f, dot(dir, triangleNormal(triangle)));

    float geometryTerm = lightDotTerm * surfaceDotTerm / (dist * dist);

    // TODO read from hit material
    BufferMaterial mat = materials[triangle.materialIndex];
    const vec3 lightIntensity = mat.emission.xyz;

    contrib = brdf * lightIntensity * geometryTerm / pdf;
}