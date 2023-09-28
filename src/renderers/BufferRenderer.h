
#ifndef RAYTRACER_BUFFERRENDERER_H
#define RAYTRACER_BUFFERRENDERER_H


#include "HostBuffer.h"
#include "glWrappers/Shader.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/ArrayBuffer.h"
#include <memory>

class BufferRenderer {

private:
    const glm::vec3 VERTICES[4] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0)};

    Shader mShader;

    std::shared_ptr<VertexArray> mVa;
    std::shared_ptr<ArrayBuffer> mVerts;

public:
    BufferRenderer();

    void reloadShaders();

    void render(HostBuffer *buffer);

};


#endif //RAYTRACER_BUFFERRENDERER_H
