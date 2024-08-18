#include "res/shaders/utils/RayTypes.glsl"
#include "res/shaders/utils/Constants.glsl"
#include "res/shaders/utils/MeshTypes.glsl"
#include "res/shaders/utils/rng.glsl"


vec3 sampleHemisphereUniform(inout uint seed) {
    /*
    float Xi1 = rand(seed);
    seed = seed * vec2(Xi1 + 0.5, 1);
    float Xi2 = rand(seed);
    seed = seed * vec2(Xi2 + 0.5, Xi1 + 0.5);
*/

    float Xi1 = randUniform_0_1(seed);
    float Xi2 = randUniform_0_1(seed);

    float theta = acos(Xi1);
    float phi = 2 * PI * Xi2;

    float z = cos(theta);
    vec2 xy = sin(theta) * vec2(cos(phi), sin(phi));
    return vec3(xy, z);
}

vec3 getRandVec3(inout vec2 seed) {
    float x = rand(seed);
    seed = seed * vec2(x + 0.5, 1);
    float y = rand(seed);
    seed = seed * vec2(y + 0.5, x + 0.5);
    float z = rand(seed);
    seed = seed * vec2(z + 0.5, y + 0.5);

    return vec3(x, y, z);
}

vec3 getNewDirection(inout uint seed, vec3 normal) {
    vec3 tangent = vec3(normal.y, normal.z, -normal.x);
    vec3 cotangent = cross(normal, tangent);

    mat3 rot = mat3(tangent, cotangent, normal);
    vec3 tangentSpaceDir = sampleHemisphereUniform(seed);

    return normalize(rot * tangentSpaceDir);
}

vec3 sampleTrianglePos(Triangle triangle, uint seed, inout float pdf) {
    float xi1 = randUniform_0_1(seed);
    float xi2 = randUniform_0_1(seed);

    if (xi1 + xi2 > 1) {
        xi1 = 1 - xi1;
        xi2 = 1 - xi2;
    }

    vec3 edge1 = triangle.V1.pos.xyz - triangle.V0.pos.xyz;
    vec3 edge2 = triangle.V2.pos.xyz - triangle.V0.pos.xyz;

    float area = 0.5 * length(cross(edge1, edge2));
    pdf = 1 / area;

    return triangle.V0.pos.xyz + xi1 * edge1 + xi2 * edge2;
}