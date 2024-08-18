#pragma once

#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "scene/Scene.h"
#include "scene/camera/Camera.h"


class Rasterizer : public ImGuiSidebarTab {

private:
    ShaderProgram mShader;
    ShaderProgram mGeometryPhaseShader;

    std::shared_ptr<Scene> mScene = nullptr;
public:
    Rasterizer();

    void render();

    void saveToBuffers(Camera &pathTraceCamera);

    void renderPaths();
    void renderCameraFrustum(CameraVisBuffers &buffers);

    void setScene(std::shared_ptr<Scene> newScene);
    std::shared_ptr<Scene> getScene();

    void reloadShaders();

    bool canRasterize();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
};
