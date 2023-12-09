#pragma once

#include "scene/Scene.h"
#include "glWrappers/IndexBuffer.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/shaders/Shader.h"


class Rasterizer {

private:
    Shader mShader;
    Shader mPathVisShader;

    Scene mScene;

    VertexArray mVa;
    IndexBuffer mIb;


public:
    Rasterizer();


    void render();
    void renderPaths();
    void renderCameraFrustum(CameraVisBuffers &buffers);

    void setScene(Scene &scene);
    Scene &getSceneRef();

    bool canRasterize();
};
