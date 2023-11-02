#include "res/shaders/utils/typesAndValues.glsl"

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 sampleHemisphereUniform(inout vec2 seed) {
    float Xi1 = rand(seed);
    seed = seed * vec2(Xi1 + 0.5, 1);
    float Xi2 = rand(seed);
    seed = seed * vec2(Xi2 + 0.5, Xi1 + 0.5);

    float theta = acos(Xi1);
    float phi = 2 * PI * Xi2;

    float z = cos(theta);
    vec2 xy = sin(theta) * vec2(cos(phi), sin(phi));
    return vec3(xy, z);
}

vec3 getNewDirection(inout vec2 seed, vec3 normal) {
    vec3 tangent = vec3(-normal.y, normal.x, normal.z);
    vec3 cotangent = cross(tangent, normal);

    mat3 rot = mat3(tangent, cotangent, normal);
    vec3 tangentSpaceDir = sampleHemisphereUniform(seed);

    return rot * tangentSpaceDir;
}

Ray samplePathStart(vec2 texelCoord, vec2 viewPortSize) {
    vec2 halfSizes = vec2(gl_NumWorkGroups.xy / 2);

    vec2 seed = frameSeed + gl_GlobalInvocationID.xy;

    float fov = PI / 2;
    float aspect = 9.0/16.0;

    float sideDist = tan(fov / 2);
    float upDist = sideDist * aspect;

    const vec3 camDir = vec3(0, 0, -1);
    const vec3 upDir = vec3(0, 1, 0);
    const vec3 tanDir = cross(camDir, upDir);

    vec2 shiftedScaled = (texelCoord - halfSizes) / halfSizes;
    Ray ray;

    float pixelSize = 2 * sideDist / viewPortSize.x;

    ray.origin = vec3(0, 0, 35) + vec3(rand(seed) * pixelSize, rand(seed) * pixelSize, 0);
    ray.direction = normalize(camDir + upDist * shiftedScaled.y * upDir + sideDist * shiftedScaled.x * tanDir);

    return ray;
}

vec3 sampleTrianglePos(Triangle triangle, vec2 seed) {
    float xi1 = rand(seed);
    float xi2 = rand(seed);

    if (xi1 + xi2 > 1) {
        xi1 = 1 - xi1;
        xi2 = 1 - xi2;
    }

    vec3 edge1 = triangle.P1 - triangle.P0;
    vec3 edge2 = triangle.P2 - triangle.P0;

    return triangle.P0 + xi1 * edge1 + xi2 * edge2;
}