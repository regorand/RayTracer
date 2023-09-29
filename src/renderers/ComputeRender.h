#pragma once

#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"

inline void computeSimple(Texture *texture, ComputeShader *shader) {
    texture->bindAsImage(0);
    shader->bind();

    glDispatchCompute(texture->getWidth(), texture->getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}