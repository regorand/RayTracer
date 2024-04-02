#include "GLBuffer.h"

#include <iostream>
#include "ShaderStorageBuffer.h"
#include "utils/glewIncludeUtils.h"


GLBuffer::GLBuffer(GLenum targetType, GLenum usage, unsigned int size, void *data)
    : mTargetType(targetType), mUsageHint(usage), mBufferSize(size)
{
    glGenBuffers(1, &m_GlId);
    glBindBuffer(mTargetType, m_GlId);

    glBufferData(mTargetType, size, data, usage);
    valid = true;
}

GLBuffer::GLBuffer()
    :mTargetType(0), mUsageHint(0), mBufferSize(0)
{
    m_GlId = -1;
    valid = false;
}

GLBuffer::~GLBuffer() {
    if (valid) {
        glDeleteBuffers(1, &m_GlId);
    }
}

GLenum GLBuffer::getTargetType() {
    return mTargetType;
}

unsigned int GLBuffer::getGlId()
{
    return m_GlId;
}

void GLBuffer::bindToTarget(GLuint target)
{
    if (valid) {
        glBindBufferBase(mTargetType, target, m_GlId);
    }
}

void GLBuffer::unbind()
{
    if (valid) {
        glBindBuffer(mTargetType, 0);
    }
}

void GLBuffer::uploadData(void *data, unsigned int size) {
    if (valid) {
        if (size != mBufferSize) {
            std::cout << "Tries to update data of buffer with incorrect buffer size" << std::endl;
        }

        bindBuffer();
        glBufferData(mTargetType, mBufferSize, data, mUsageHint);

        unbind();
    }
}

void GLBuffer::updateData(void *data, unsigned int size, unsigned int offset) {
    if (valid) {

    }
    glNamedBufferSubData(m_GlId, offset, size, data);
}
void GLBuffer::getData(void *target, unsigned int size, unsigned int offset) {
    if (valid) {
        glGetNamedBufferSubData(m_GlId, offset, size, target);
    }
}

void GLBuffer::bindBuffer() {
    if (valid) {
        glBindBuffer(mTargetType, m_GlId);
    }
}

unsigned int GLBuffer::getBuffersSize() {
    return mBufferSize;
}

bool GLBuffer::isValid() {
    return valid;
}
