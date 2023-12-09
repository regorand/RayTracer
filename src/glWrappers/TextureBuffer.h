#pragma once

#include <cstddef>
#include "GL/glew.h"

class TextureBuffer
{
private:
    unsigned int mBufferSize;
    GLenum mInternalFormat;
    GLenum mUsageHint;

    unsigned int mTexBufGlId;
    unsigned int mBufObjGlId;

public:
    TextureBuffer(unsigned int size, GLenum internalFormat, GLenum usage, void *data = nullptr);
    ~TextureBuffer();

    void uploadData(void *data, unsigned int size);
    void updateData(void *data, unsigned int size, unsigned int offset);

    void getData(void *target, unsigned int size, unsigned int offset);

    void bind(GLuint target);
    void unbind();

private:
    void bindBuffer();
    void unbindBuffer();

};

