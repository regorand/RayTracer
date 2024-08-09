#include <iostream>
#include "ShaderStorageBuffer.h"

ShaderStorageBuffer::ShaderStorageBuffer()
    :m_GlId(-1) { }

ShaderStorageBuffer::ShaderStorageBuffer(unsigned int size, GLenum usage, void* data)
	:mBufferSize(size), mUsageHint(usage)
{
	glGenBuffers(1, &m_GlId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_GlId);

    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void ShaderStorageBuffer::destroy() {
    glDeleteBuffers(1, &m_GlId);
}

unsigned int ShaderStorageBuffer::getGlId()
{
	return m_GlId;
}

void ShaderStorageBuffer::bindToTarget(GLuint target)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, target, m_GlId);
}

void ShaderStorageBuffer::unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::uploadData(void *data, unsigned int size) {
    if (size != mBufferSize) {
        std::cout << "Tries to update data of buffer with incorrect buffer size" << std::endl;
    }

    bindBuffer();
    glBufferData(GL_SHADER_STORAGE_BUFFER, mBufferSize, data, mUsageHint);

    unbind();
}

void ShaderStorageBuffer::updateData(void *data, unsigned int size, unsigned int offset) {
    glNamedBufferSubData(m_GlId, offset, size, data);
}
void ShaderStorageBuffer::getData(void *target, unsigned int size, unsigned int offset) {
    glGetNamedBufferSubData(m_GlId, offset, size, target);
}

void ShaderStorageBuffer::bindBuffer() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_GlId);
}

unsigned int ShaderStorageBuffer::getBuffersSize() {
    return mBufferSize;
}
