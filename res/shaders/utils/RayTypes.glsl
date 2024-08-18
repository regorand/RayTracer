#include "res/shaders/utils/MeshTypes.glsl"

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct RayHit {
    float dist;
    vec3 position;
    vec3 normal;
    int matID;
    bool hitBackface;
};