#pragma once

#include <string>
#include "HostBuffer.h"
#include "GL/glew.h"

class Texture
{
private:
    unsigned int m_GlId;

    std::string m_FilePath;
    bool mLoadedFromFile = false;

    int m_bytesPerPixel, m_width, m_height;
    unsigned char* m_Buf;


public:
    Texture(const std::string& path);
    Texture(HostBuffer *buffer);
    ~Texture();

    void Bind(unsigned int textureSlot = 0);
    void Unbind();

private:
    void createTexture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data);

};

