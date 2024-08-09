
#include <memory>
#include "BufferRenderer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/rasterization/ArrayBuffer.h"
#include "glWrappers/TextureBuffer.h"
#include "glWrappers/Texture.h"
#include "imgui.h"
#include "glWrappers/shaders/ShaderUtils.hpp"

BufferRenderer::BufferRenderer()
    : mShader(makeRasterizeProgram("res/shaders/raster/fullscreen.vert", "res/shaders/raster/fullscreen.frag")),
      mStorageDisplayShader(makeRasterizeProgram("res/shaders/raster/fullscreen.vert", "res/shaders/raster/displayStorageBuffer.frag")),
        mVa(std::make_shared<VertexArray>()),
        mVerts(std::make_shared<ArrayBuffer>((void *) VERTICES, 4 * 3 * sizeof(float), true))
{

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

}

void BufferRenderer::reloadShaders() {
    mShader.reloadShader();
    mStorageDisplayShader.reloadShader();
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
void BufferRenderer::renderStorageBuffer(ShaderStorageBuffer &buffer,
                                         glm::vec2 extents) {

    mVa->bind();
    mVerts->bind();

    mStorageDisplayShader.bind();

    mStorageDisplayShader.setUniform1i("width", static_cast<int>(extents.x));
    mStorageDisplayShader.setUniform1i("totalBufferSize", buffer.getBuffersSize());

    buffer.bindToTarget(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mVa->unbind();
    mVerts->unbind();
    mShader.unbind();
}

void BufferRenderer::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    buildImGuiWindow();
}
