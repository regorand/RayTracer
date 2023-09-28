
#include <memory>
#include "BufferRenderer.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/ArrayBuffer.h"
#include "glWrappers/Shader.h"
#include "glWrappers/TextureBuffer.h"
#include "glWrappers/Texture.h"

BufferRenderer::BufferRenderer()
    : mShader(Shader("res/shaders/fullscreen.vert", "res/shaders/fullscreen.frag")),
        mVa(std::make_shared<VertexArray>()),
        mVerts(std::make_shared<ArrayBuffer>((void *) VERTICES, 4 * 3 * sizeof(float), true))
{

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

}

void BufferRenderer::render(HostBuffer *buffer) {

    // TODO: is it ok to do create buffer on demand if goal of Class is to only occasionally render host buffer \
        -> infact, is there another way ?

    Texture texture(buffer);

    mVa->bind();
    mVerts->bind();

    mShader.bind();
    texture.Bind(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mVa->unbind();
    mVerts->unbind();
    mShader.unbind();
}

void BufferRenderer::reloadShaders() {
    mShader.reloadShader();
}
