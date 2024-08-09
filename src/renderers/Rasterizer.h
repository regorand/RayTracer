#pragma once

#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "scene/Scene.h"
#include "scene/camera/Camera.h"


class Rasterizer {

private:
    ShaderProgram mShader;
    ShaderProgram mPathVisShader;
    ShaderProgram mGeometryPhaseShader;

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
