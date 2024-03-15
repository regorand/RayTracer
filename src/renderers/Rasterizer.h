#pragma once

#include "scene/Scene.h"
#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/shaders/Shader.h"
#include "glWrappers/Texture.h"


class Rasterizer {

private:
    Shader mShader;
    Shader mPathVisShader;
    Shader mGeometryPhaseShader;

    Scene mScene;

    VertexArray mVa;
    IndexBuffer mIb;


public:
    Rasterizer();


    void render();

    void saveToBuffers(Camera &pathTraceCamera);

    void renderPaths();
    void renderCameraFrustum(CameraVisBuffers &buffers);

    void setScene(Scene &scene);
    Scene &getSceneRef();

    bool canRasterize();
};
