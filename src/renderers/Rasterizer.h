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
    ShaderProgram mCameraVisShader;

    std::shared_ptr<Scene> mScene = nullptr;

    bool renderCamFrustums = false;
public:
    Rasterizer();

    void render(int width, int height);

    void renderCameraFrustum(Camera &camera, bool isHovered);

    void setScene(std::shared_ptr<Scene> newScene);
    std::shared_ptr<Scene> getScene();

    void reloadShaders();

    bool canRasterize();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
};
