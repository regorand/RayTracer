#include "res/shaders/utils/RayTypes.glsl"

Ray samplePathStart(vec2 texelCoord, vec2 viewPortSize) {
    vec2 halfSizes = vec2(viewPortSize / 2);

    // vec2 seed = frameSeed + gl_GlobalInvocationID.xy;

    float upDist = tan(fov / 2.0f);
    float sideDist = upDist * aspect;

    const vec3 camDir = viewDir;
    const vec3 upDir = upDir;
    const vec3 tanDir = cross(camDir, upDir);

    vec2 shiftedScaled = (texelCoord - halfSizes) / halfSizes;
    Ray ray;

    float pixelSize = 2 * sideDist / viewPortSize.x;

    ray.origin = cameraPos;//  + vec3(rand(seed) * pixelSize, rand(seed) * pixelSize, 0);
    ray.direction = normalize(camDir + upDist * shiftedScaled.y * upDir + sideDist * shiftedScaled.x * tanDir);

    return ray;
}

Ray samplePathStart2(vec2 texelCoord, vec2 viewPortSize, uint seed) {
    vec2 halfSizes = vec2(viewPortSize / 2);

    float upDist = tan(fov / 2.0f);
    float sideDist = upDist * aspect;

    const vec3 camDir = viewDir;
    const vec3 upDir = upDir;
    const vec3 tanDir = cross(camDir, upDir);

    vec2 shiftedScaled = (texelCoord - halfSizes) / halfSizes;
    Ray ray;

    float pixelSize = 2 * sideDist / viewPortSize.x;

    ray.origin = cameraPos;//  + vec3(rand(seed) * pixelSize, rand(seed) * pixelSize, 0);
    ray.direction = normalize(camDir + upDist * shiftedScaled.y * upDir + sideDist * shiftedScaled.x * tanDir);

    return ray;
}