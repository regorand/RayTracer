#include "res/shaders/utils/RayTypes.glsl"

Ray samplePathStart(vec2 texelCoord, vec2 viewPortSize, inout uint seed) {
    vec2 halfSizes = vec2(viewPortSize / 2);

    float upDist = tan(fov / 2.0f);
    float sideDist = upDist * aspect;

    const vec3 camDir = viewDir;
    const vec3 upDir = upDir;
    const vec3 tanDir = cross(camDir, upDir);

    vec2 shiftedScaled = (texelCoord - halfSizes) / halfSizes;
    Ray ray;

    float pixelSize = upDist / viewPortSize.y;

    ray.origin = cameraPos;
    ray.direction = normalize(camDir
                                 + upDist * (shiftedScaled.y + randUniform_neg_1_to_1(seed) * pixelSize) * upDir
                                 + sideDist * (shiftedScaled.x + randUniform_neg_1_to_1(seed) * pixelSize) * tanDir);


    return ray;
}

Ray samplePathStart2(vec2 texelCoord, vec2 viewPortSize, inout uint seed) {
    vec2 halfSizes = vec2(viewPortSize / 2);

    float upDist = tan(fov / 2.0f);
    float sideDist = upDist * aspect;

    const vec3 camDir = normalize(viewDir);
    const vec3 upDir = normalize(upDir);
    const vec3 tanDir = cross(camDir, upDir);

    vec2 sampledCoord = vec2(randUniform_uint_0_n(seed, width), randUniform_uint_0_n(seed, height));
    vec2 shiftedScaled = (sampledCoord - halfSizes) / halfSizes;
    Ray ray;

    float pixelSize = 2 * upDist / viewPortSize.y;

    float diffY = randUniform_0_1(seed) - 0.5f;
    float diffX = randUniform_0_1(seed) - 0.5f;

    ray.origin = cameraPos;
    ray.direction = normalize(camDir
                                + upDist * shiftedScaled.y * upDir
                                + sideDist * shiftedScaled.x * tanDir);

    return ray;
}