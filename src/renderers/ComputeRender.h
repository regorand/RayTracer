#pragma once

#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"

inline void computeSimple(Texture *texture, ComputeShader *shader) {
    texture->bindAsImage(0);
    shader->bind();

    std::mt19937 mt(time(nullptr));
    std::uniform_real_distribution<> dis(0.0, 1.0);

    glm::vec2 seed(dis(mt), dis(mt));

    shader->setUniform2f("frameSeed", seed);

    glDispatchCompute(texture->getWidth(), texture->getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

// inline void computeSingleBounceShader(SingleBounceRessources *res, ComputeShader *shader);