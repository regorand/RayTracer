#include "res/shaders/scenes/CPUScene.glsl"
#include "res/shaders/utils/MeshTypes.glsl"
#include "res/shaders/utils/samplingUtils.glsl"

MeshObject sampleNEEObject(inout uint seed) {
    // TODO: this is naive implementation, do some kinf of precompute

    // TODO dont uniformly sample light sources

    uint lightSourceIndex = emissiveIndices[randUniform_uint_0_n(seed, emissiveCount)];

    return meshObjects[lightSourceIndex];
}

Triangle sampleNEETriangle(MeshObject object, inout uint seed) {
    // TODO also here, dont uniformly sample, improve

    uint triangleIndex = object.firstTriangleIndex + randUniform_uint_0_n(seed, object.triangleCount);

    return triangles[triangleIndex];
}

void sampleLightsourceNEE(vec3 pos, vec3 N, vec3 inDir, Material material, inout uint seed, out vec3 contrib, out float pdf) {
    // TODO sample light source
    /*
        1. Need buffer data (duh)
        2. compute importance from geometry term (distance, projected area) and emitted radiance
        4. normalize object area by sum of objects
        do this by resampled importance sampling or reservoir sampling (forgot which one, look up)

        Then to sample triangle on object
        1. use projected triangle area
        2. normalize pdf by estimated object area
    */

    MeshObject object = sampleNEEObject(seed);

    Triangle triangle = sampleNEETriangle(object, seed);

    vec3 sampledPos = sampleTrianglePos(triangle, seed);
    // TODO sample point on triangle

    // TODO shoot shadow ray
    /*
        1. Implement method in rayTraceUtils, use fact that we can give max distance
    */

    vec3 dir = sampledPos - pos;
    float dist = length(dir);
    dir = dir / vec3(dist);

    bool shadowed = anyHit(Ray(pos, dir), dist);

    if (shadowed) {
        contrib = vec3(0);
        pdf = 1;
        return;
    }

    // TODO calculate pdf
/*
        1. calculate pdf to sample object
        2. calculate pdf to sample triangle on object
    */
    pdf = 1;


    vec3 halfway = (dir + inDir) / vec3(2);
    float thetaM = acos(dot(halfway, N));

    // TODO read from hit material
    const vec3 lightIntensity = vec3(15);

    vec3 brdf;
    evaluateBRDF(inDir, N, halfway, thetaM, dir, material, brdf);

    // contrib = brdf * lightIntensity;
    // TODO calculate light contrib
    /*
        1. compute halfway vector
        2. either explicitely compute NDF or create overridden function evaluate BRDF, which does actually evaluate the NDF itself
        3. multiply by light intensity (and geometry term is that cancelled out if I consider it in importance sampling ?
    */
}