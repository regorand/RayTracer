#include "TextureBuffer.h"
#include "utils/glewIncludeUtils.h"


TextureBuffer::TextureBuffer(unsigned int size, GLenum internalFormat, GLenum usage, void *data)
    : mBufferSize(size), mInternalFormat(internalFormat), mUsageHint(usage)
{
    glGenBuffers(1, &mBufObjGlId);

    bindBuffer();
    glBufferData(GL_TEXTURE_BUFFER, mBufferSize, data, mUsageHint);

    glGenTextures(1, &mTexBufGlId);
    glBindTexture(GL_TEXTURE_BUFFER, mTexBufGlId);

    glTexBuffer(GL_TEXTURE_BUFFER, mInternalFormat, mBufObjGlId);
}


TextureBuffer::~TextureBuffer()
{
    glDeleteTextures(1, &mTexBufGlId);
    glDeleteBuffers(1, &mBufObjGlId);
}

void TextureBuffer::bind(GLuint target)
{
    bindBuffer();

    GLint param = 0;
    glGetTextureParameteriv(mTexBufGlId, GL_IMAGE_FORMAT_COMPATIBILITY_TYPE, &param);
    GLenum error = glGetError();

    glBindImageTexture(target, mTexBufGlId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

void TextureBuffer::unbind()
{
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void TextureBuffer::uploadData(void *data, unsigned int size) {
    if (size != mBufferSize) {
        std::cout << "Tries to update data of buffer with incorrect buffer size" << std::endl;
    }

    bindBuffer();
    glBufferData(GL_TEXTURE_BUFFER, mBufferSize, data, mUsageHint);

    unbind();
}

void TextureBuffer::updateData(void *data, unsigned int size, unsigned int offset) {
    glNamedBufferSubData(mBufObjGlId, offset, size, data);
}

void TextureBuffer::getData(void *target, unsigned int size, unsigned int offset) {
    glGetNamedBufferSubData(mBufObjGlId, offset, size, target);
}

void TextureBuffer::bindBuffer() {
    glBindBuffer(GL_TEXTURE_BUFFER, mBufObjGlId);
}