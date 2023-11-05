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

#define TRIANGLE_BUFFER_BINDING 0
#define OBJECT_BUFFER_BINDING 1
#define MATERIAL_BUFFER_BINDING 2
#define EMISSIVE_BUFFER_BINDING 3