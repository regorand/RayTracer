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