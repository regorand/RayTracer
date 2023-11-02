#include <iostream>
#include "Texture.h"
#include "GL/glew.h"

Texture::Texture(int width, int height)
        : mGlId(),
          mFilePath(""),
          mLoadedFromFile(false),
          m_bytesPerPixel(0),
          mWidth(width),
          mHeight(height),
          mFormat(GL_RGB),
          mInternalFormat(GL_RGBA32F),
          mType(GL_FLOAT),
          mAccessType(GL_READ_WRITE) {

}

Texture::Texture(HostBuffer *buffer)
        : mGlId(),
          mFilePath(""),
          mLoadedFromFile(false),
          m_bytesPerPixel(0),
          mWidth(buffer->getWidth()),
          mHeight(buffer->getHeight()),
          mFormat(GL_RGB),
          mInternalFormat(GL_RGBA32F),
          mType(GL_FLOAT),
          mAccessType(GL_READ_WRITE) {
    createTexture(mInternalFormat, mWidth, mHeight, mFormat, mType, buffer->getData());
}

Texture::~Texture() {
    glDeleteTextures(1, &mGlId);
}

void Texture::bind(unsigned int textureSlot) {
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, mGlId);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createTexture(GLint internalFormat,
                            GLsizei width,
                            GLsizei height,
                            GLenum format,
                            GLenum type,
                            void *data) {
    glGenTextures(1, &mGlId);
    glBindTexture(GL_TEXTURE_2D, mGlId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
    unbind();
}

void Texture::updateTexture(void *pixels) {
    updateTexture(0, 0, mWidth, mHeight, pixels);
}

void Texture::updateTexture(GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, void *pixels) {
    unbind();
    bind(0);
    glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, mFormat, mType, pixels);
}

void Texture::bindAsImage(unsigned int imageSlot) {
    glBindImageTexture(imageSlot, mGlId, 0, GL_FALSE, 0, mAccessType, mInternalFormat);
}

unsigned int Texture::getWidth() {
    return (unsigned int) mWidth;
}

unsigned int Texture::getHeight() {
    return (unsigned int) mHeight;
}

void Texture::setAccessType(GLenum newType) {
    if (newType != GL_READ_WRITE && newType != GL_READ_ONLY && newType != GL_WRITE_ONLY) {
        std::cerr << "Trying to set access type of texture buffer to invalid value " << newType << std::endl;
        return;
    }
    mAccessType = newType;
}

GLenum Texture::getAccessType() {
    return mAccessType;
}
