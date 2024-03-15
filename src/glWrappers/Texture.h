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

    /*
     * Access Type when the texture is bound as a buffer object
     */
    GLenum mAccessType;
public:
    Texture(int mWidth, int mHeight, GLenum format, GLint internalFormat, GLenum type, GLenum accessType);
    Texture(const std::string& path);
    Texture(HostBuffer *buffer);

    void destroy();

    void updateTexture(void *pixels);
    void updateTexture(GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, void *pixels);

    unsigned int getWidth();
    unsigned int getHeight();

    unsigned int getGlID();

    void setAccessType(GLenum newType);
    GLenum getAccessType();

    void bind(unsigned int textureSlot = 0);
    void unbind();

    void bindAsImage(unsigned int imageSlot = 0);

    // bool readPixels(glm::vec2 coords, glm::vec2 coords, glm::vec4 *);

private:
    void createTexture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data);

};

