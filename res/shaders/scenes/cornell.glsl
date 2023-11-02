#include "res/shaders/utils/typesAndValues.glsl"

const int NUM_SPHERES = 2;

const vec3 RED = vec3(1, 0.1, 0.1);
const vec3 GREEN = vec3(0.1, 1, 0.1);
const vec3 BLUE = vec3(0.1, 0.1, 1);
const vec3 WHITE = vec3(1);

const Sphere spheres[NUM_SPHERES] = Sphere[NUM_SPHERES](
    Sphere(vec3(0, 0, -10), 10, Material(RED, true, false, false, 1.52f)),
    Sphere(vec3(-8, -6, 5), 5, Material(BLUE, true, false, true, 1.52f))
);


const float BOX_SIZE = 20; // Half of side length of cube making cornell box

const vec3 V0 = vec3(-BOX_SIZE, -BOX_SIZE, -BOX_SIZE);
const vec3 V1 = vec3(BOX_SIZE, -BOX_SIZE, -BOX_SIZE);
const vec3 V2 = vec3(-BOX_SIZE, BOX_SIZE, -BOX_SIZE);
const vec3 V3 = vec3(BOX_SIZE, BOX_SIZE, -BOX_SIZE);

const vec3 V4 = vec3(-BOX_SIZE, -BOX_SIZE, BOX_SIZE);
const vec3 V5 = vec3(BOX_SIZE, -BOX_SIZE, BOX_SIZE);
const vec3 V6 = vec3(-BOX_SIZE, BOX_SIZE, BOX_SIZE);
const vec3 V7 = vec3(BOX_SIZE, BOX_SIZE, BOX_SIZE);

float scaleFac = 0.8;
vec3 scaleVec = vec3(scaleFac, 1, scaleFac);
vec3 displacement = vec3(0, -0.1, 0);

const vec3 V8 = V2 * scaleVec + displacement;
const vec3 V9 = V3 * scaleVec + displacement;
const vec3 V10 = V6 * scaleVec + displacement;
const vec3 V11 = V7 * scaleVec + displacement;

const int NUM_TRIANGLES = 12;

const Triangle triangles[NUM_TRIANGLES] = Triangle[NUM_TRIANGLES](
// Back Wall
    Triangle(V0, V1, V2,
             Material(WHITE, true, false, false, 1.0f)
    ),

    Triangle(V1, V3, V2,
             Material(WHITE, true, false, false, 1.0f)
    ),

// Right Wall
    Triangle(V1, V5, V7,
             Material(GREEN, true, false, false, 1.0f)
    ),

    Triangle(V1, V7, V3,
             Material(GREEN, true, false, false, 1.0f)
    ),

// Left Wall
    Triangle(V0, V2, V6,
             Material(RED, false, false, false, 1.0f)
    ),

    Triangle(V0, V6, V4,
             Material(RED, false, false, false, 1.0f)
    ),

// Bottom
    Triangle(V0, V5, V1,
             Material(BLUE, true, false, false, 1.0f)
    ),

    Triangle(V0, V4, V5,
             Material(BLUE, true, false, false, 1.0f)
    ),

// Top
    Triangle(V2, V3, V7,
             Material(WHITE, true, false, false, 1.0f)
    ),

    Triangle(V2, V7, V6,
             Material(WHITE, true, false, false, 1.0f)
    ),

// Light
    Triangle(V8, V9, V10,
             Material(WHITE, true, true, false, 1.0f)
    ),

    Triangle(V9, V11, V10,
             Material(WHITE, true, true, false, 1.0f)
    )
);

const int LIGHT_SOURCE_COUNT = 2;

const int[LIGHT_SOURCE_COUNT] LIHGT_INDICES = int[LIGHT_SOURCE_COUNT](10, 11);