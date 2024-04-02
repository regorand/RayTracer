#pragma once

#include "scene/OldScene.h"
#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/shaders/ShaderOld.h"
#include "glWrappers/Texture.h"


class Rasterizer {

private:
    ShaderOld mShader;
    ShaderOld mPathVisShader;
    ShaderOld mGeometryPhaseShader;

    OldScene mScene;

    VertexArray mVa;
    IndexBuffer mIb;


public:
    Rasterizer();


    void render();

    void saveToBuffers(Camera &pathTraceCamera);

    void renderPaths();
    void renderCameraFrustum(CameraVisBuffers &buffers);

    void setScene(OldScene &scene);
    OldScene &getSceneRef();

    bool canRasterize();
};
