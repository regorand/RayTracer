
#include <iostream>
#include "SSBO.h"

SSBO::SSBO()
        : mData(nullptr), mDataSize(0), mUsage(GL_READ_WRITE), mTargetBinding(0)
{
    glGenBuffers(1, &mGLId);
    mHasData = false;
}

SSBO::SSBO(void *data, size_t dataSize, GLenum usage, GLuint targetBinding)
    : mData(data), mDataSize(dataSize), mUsage(usage), mTargetBinding(targetBinding)
{
    glGenBuffers(1, &mGLId);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mGLId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mDataSize, mData, mUsage);

    mHasData = true;
}

void SSBO::bind() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mTargetBinding, mGLId);
}

void SSBO::unbind() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
