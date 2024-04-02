
#ifndef RAYTRACER_BUFFERRENDERER_H
#define RAYTRACER_BUFFERRENDERER_H

#include "HostBuffer.h"
#include "glWrappers/rasterization/ArrayBuffer.h"
#include "glWrappers/ShaderStorageBuffer.h"
#include "glWrappers/Texture.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/shaders/ShaderOld.h"
#include "gui/ImGuiSidebarTab.h"
#include <memory>

class BufferRenderer : public ImGuiSidebarTab  {

private:
    const glm::vec3 VERTICES[4] = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0)};

    ShaderOld mShader;
    ShaderOld mStorageDisplayShader;

    std::shared_ptr<VertexArray> mVa;
    std::shared_ptr<ArrayBuffer> mVerts;

public:
    BufferRenderer();

    void reloadShaders();

    void render(HostBuffer *buffer);
    void render(Texture *texture);

    void renderStorageBuffer(ShaderStorageBuffer &buffer, glm::vec2 extents);

    void buildImGuiWindow();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;


};


#endif //RAYTRACER_BUFFERRENDERER_H
