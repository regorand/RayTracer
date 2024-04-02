#pragma once

#include "scene/OldScene.h"
#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ShaderProgram.h"


class Rasterizer {

private:
    ShaderProgram mShader;
    ShaderProgram mPathVisShader;
    ShaderProgram mGeometryPhaseShader;

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
