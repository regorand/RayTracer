#pragma once

#include "GL/glew.h"

class ShaderStorageBuffer
{
private:
    unsigned int mBufferSize;

    GLenum mUsageHint;

    unsigned int m_GlId;
public:
    ShaderStorageBuffer(unsigned int size, GLenum usage, void *data = nullptr);
    ~ShaderStorageBuffer();

    void uploadData(void *data, unsigned int size);
    void updateData(void *data, unsigned int size, unsigned int offset);

    void getData(void *target, unsigned int size, unsigned int offset);

    unsigned int getGlId();

    void bindBuffer();

    void bindToTarget(GLuint target);
    void unbind();

    unsigned int getBuffersSize();
};

