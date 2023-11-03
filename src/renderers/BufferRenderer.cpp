
#include <memory>
#include "BufferRenderer.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/ArrayBuffer.h"
#include "glWrappers/shaders/Shader.h"
#include "glWrappers/TextureBuffer.h"
#include "glWrappers/Texture.h"
#include "imgui.h"

BufferRenderer::BufferRenderer()
    : mShader(Shader("res/shaders/raster/fullscreen.vert", "res/shaders/raster/fullscreen.frag")),
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
    texture.bind(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mVa->unbind();
    mVerts->unbind();
    mShader.unbind();
}

void BufferRenderer::reloadShaders() {
    mShader.reloadShader();
}

void BufferRenderer::render(Texture *texture) {
    mVa->bind();
    mVerts->bind();

    mShader.bind();
    texture->bind(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mVa->unbind();
    mVerts->unbind();
    mShader.unbind();
}

void BufferRenderer::buildImGuiWindow() {
    if (ImGui::CollapsingHeader("Texture Renderer Control")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reload display shaders", buttonSize)) {
            reloadShaders();
        }
    }

}
