
#pragma once


#include "tiny_gltf.h"
#include "glWrappers/shaders/SSBO.h"

class PathTraceBuffers {
    bool mIsValid;

    SSBO mTriangleSSBO;
    SSBO mObjectSSBO;
    SSBO mMaterialSSBO;
    SSBO mEmissiveSSBO;


public:
    PathTraceBuffers();

    bool isValid();

    void bind();
    void unbind();

};
