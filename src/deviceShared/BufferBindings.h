#ifdef __cplusplus
#pragma once
using uint = unsigned int;
#endif

#ifdef __cplusplus  // Descriptor binding helper for C++ and GLSL
#define START_BINDING(a) enum a {
#define END_BINDING() }
#define ALIGN_AS(a) alignas(a)
#else
#define START_BINDING(a) const uint
    #define END_BINDING()
    #define ALIGN_AS(a)
#endif

#define TRIANGLE_BUFFER_BINDING (0)
#define OBJECT_BUFFER_BINDING (1)
#define MESH_BUFFER_BINDING (2)
#define MATERIAL_BUFFER_BINDING (3)
#define EMISSIVE_BUFFER_BINDING (4)
#define TRANSFORM_BUFFER_BINDING (5)
#define BVH_NODES_BUFFER_BINDING (6)
#define TRIANGLE_INDICES_BUFFER_BINDING (7)
#define POS_BUFFER_TARGET (5)
#define DIR_BUFFER_TARGET (6)
#define MULT_BUFFER_TARGET (7)

#define POST_PROC_SOURCE_BINDING (0)
#define POST_PROC_TARGET_BINDING (1)

#define RAST_POS_BUFFER_BINDING (2)
#define RAST_NORMAL_BUFFER_BINDING (3)
#define RAST_ALBEDO_BUFFER_BINDING (4)