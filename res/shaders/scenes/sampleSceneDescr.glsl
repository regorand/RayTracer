#include "res/shaders/utils/typesAndValues.glsl"

const vec3 lightPos = vec3(15, 15, 5);
const vec3 lightIntensity = 100 * vec3(100, 100, 100);

const int NUM_SPHERES = 5;

const Sphere spheres[NUM_SPHERES] = Sphere[NUM_SPHERES](
    Sphere(vec3(-10, -2, -10), 5, Material(vec3(0.6, 1, 0.1), true, false, true, 1.52f)),
    Sphere(vec3(15, 5, -26), 5, Material(vec3(1, 0.1, 0.8), true, false, false, 1.0f)),
    Sphere(vec3(16, -1, -15), 5, Material(vec3(0.1, 0.5, 1), true, false, false, 1.0f)),
    Sphere(vec3(-10, 200, -26), 150, Material(vec3(0.3, 1, 0), true, true, false, 1.0f)),
    Sphere(vec3(0, -1000, -30), 990, Material(0.8 * vec3(1, 0, 0), true, false, false, 1.0f))
);

const int NUM_TRIANGLES = 2;

const Triangle triangles[NUM_TRIANGLES] = Triangle[NUM_TRIANGLES](
    Triangle(vec3(10, -10, -15), vec3(-10, 10, -15), vec3(-10, -10, -15),
             Material(vec3(1, 0, 0), true, false, false, 1.0f)
    ),
    Triangle(vec3(10, -10, -15), vec3(10, 10, -15), vec3(-10, 10, -15),
             Material(vec3(0, 0, 1), true, false, false, 1.0f)
    )
);
