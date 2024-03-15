
#include "TextureFactory.h"

Texture createRWTexture(int width, int height, GLint internalFormat, GLenum format, GLenum type) {
    return {width, height, format, internalFormat, type, GL_READ_WRITE};
}

Texture createPathTraceBuffer(int width, int height) {
    return createRWTexture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
}

Texture createDepthBuffer(int width, int height) {
    return {width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_READ_WRITE};
}
