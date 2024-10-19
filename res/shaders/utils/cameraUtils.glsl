#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/utils/rng.glsl"

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
    /*
    ray.direction = normalize(camDir
                                 + upDist * (shiftedScaled.y + randUniform_neg_1_to_1(seed) * pixelSize) * upDir
                                 + sideDist * (shiftedScaled.x + randUniform_neg_1_to_1(seed) * pixelSize) * tanDir);
                                 */

    ray.direction = normalize(camDir
                                 + upDist * (shiftedScaled.y) * upDir
                                 + sideDist * (shiftedScaled.x) * tanDir);


    return ray;
}