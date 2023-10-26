#include "res/shaders/utils/types.glsl"

const int NUM_SPHERES = 5;

const Sphere spheres[NUM_SPHERES] = Sphere[NUM_SPHERES](
    Sphere(vec3(0, 0, -10), 4, Material(vec3(0.2, 1, 0.1), 1)),
    Sphere(vec3(0, 5, -15), 5, Material(vec3(1, 0.2, 0.3), 0)),
    Sphere(vec3(16, -6, -8), 2, Material(vec3(0.1, 0.5, 1), 1)),
    Sphere(vec3(-10, 10, -26), 8, Material(vec3(0.3, 1, 0), 1)),
    Sphere(vec3(0, -1000, -30), 990, Material(0.8 * vec3(1, 1, 1), 1))
);