#pragma once

#include "GL/glew.h"

class GLBuffer {

    unsigned int mBufferSize;

    GLenum mTargetType;
    GLenum mUsageHint;

    bool valid;
    unsigned int m_GlId;
public:
    GLBuffer(GLenum targetType, GLenum usage, unsigned int size, void *data = nullptr);
    GLBuffer();
    ~GLBuffer();

    bool isValid();

    GLenum getTargetType();

    void uploadData(void *data, unsigned int size);
    void updateData(void *data, unsigned int size, unsigned int offset);

    void getData(void *target, unsigned int size, unsigned int offset);

    unsigned int getGlId();

    void bindBuffer();

    void bindToTarget(GLuint target);
    void unbind();

    unsigned int getBuffersSize();
};
