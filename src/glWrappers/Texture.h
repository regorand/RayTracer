#pragma once

#include <string>
#include "HostBuffer.h"
#include "GL/glew.h"

class Texture
{
private:
    unsigned int mGlId;

    std::string mFilePath;
    bool mLoadedFromFile = false;

    int m_bytesPerPixel, mWidth, mHeight;
    GLenum mFormat;
    GLint mInternalFormat;
    GLenum mType;

    unsigned char* m_Buf;
public:
    Texture(const std::string& path);
    Texture(HostBuffer *buffer);
    ~Texture();

    void updateTexture(void *pixels);
    void updateTexture(GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, void *pixels);

    unsigned int getWidth();
    unsigned int getHeight();

    void bind(unsigned int textureSlot = 0);
    void unbind();

    void bindAsImage(unsigned int imageSlot = 0);

private:
    void createTexture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data);

};

