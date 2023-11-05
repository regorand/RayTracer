#pragma once

#include "GL/glew.h"

class SSBO {
private:
    bool mHasData;

    GLuint mGLId;

    void *mData;
    size_t mDataSize;
    GLenum mUsage;
    GLuint mTargetBinding;

public:
    SSBO();
    SSBO(void *data, size_t dataSize, GLenum usage, GLuint targetBinding);

    bool hasData() { return mHasData; };

    void bind();
    void unbind();
};