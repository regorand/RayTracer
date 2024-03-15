#pragma once

#include "Texture.h"
#include "GL/glew.h"

Texture createRWTexture(int width, int height, GLint internalFormat, GLenum format, GLenum type);

Texture createPathTraceBuffer(int width, int height);

Texture createDepthBuffer(int width, int height);