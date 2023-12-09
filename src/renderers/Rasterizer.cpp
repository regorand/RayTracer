#include "Rasterizer.h"
#include "glm/gtc/matrix_inverse.hpp"

Rasterizer::Rasterizer()
        : mShader("res/shaders/raster/simpleScene.vert", "res/shaders/raster/simpleScene.frag"),
          mPathVisShader("res/shaders/raster/rasterizePath.vert", "res/shaders/raster/rasterizePath.frag") {

}

void Rasterizer::render() {


    Camera &camera = mScene.getCamera();

    glm::mat4 cameraMat = camera.getTransformation();
    glm::mat4 projectionMat = camera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;

    glEnable(GL_DEPTH_TEST);

    for (auto &object: mScene.getObjects()) {
        mShader.bind();

        object.getVertexArray().bind();
        object.getIndexBuffer().bind();

        glm::mat4 model = object.getTransformRef().getMatrix();
        glm::mat4 MVP = VP * model;
        glm::mat4 normalTransform = glm::inverseTranspose(model);

        // mShader.setUniformMat4f("transform", model);
        mShader.setUniformMat4f("MVP", MVP);
        mShader.setUniformMat4f("normalTransform", normalTransform);

        if (object.isTransmissive()) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (object.isEmissive()) {
            mShader.setUniform1i("emissive", 1);
        }

        glDrawElements(GL_TRIANGLES, object.getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        mShader.unbind();
    }
}

void Rasterizer::renderPaths() {
    Camera &camera = mScene.getCamera();

    glm::mat4 cameraMat = camera.getTransformation();
    glm::mat4 projectionMat = camera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;


    for (auto &path: mScene.getPathObjects()) {
        mPathVisShader.bind();
        path.getVertexArray().bind();
        path.getIndexBuffer().bind();

        mPathVisShader.setUniformMat4f("VP", VP);

        glDrawElements(GL_LINES, path.getIndexBuffer().getCount(), GL_UNSIGNED_INT, nullptr);
        mPathVisShader.unbind();
    }
}

void Rasterizer::setScene(Scene &scene) {
    mScene = scene;
}

bool Rasterizer::canRasterize() {
    return mScene.canRasterize();
}

Scene &Rasterizer::getSceneRef() {
    return mScene;
}

void Rasterizer::renderCameraFrustum(CameraVisBuffers &buffers) {
    Camera &camera = mScene.getCamera();

    glm::mat4 cameraMat = camera.getTransformation();
    glm::mat4 projectionMat = camera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;


    mPathVisShader.bind();
    buffers.vertexArray.bind();
    buffers.indexBuffer.bind();

    mPathVisShader.setUniformMat4f("VP", VP);

    glDrawElements(GL_LINES, buffers.indexBuffer.getCount(), GL_UNSIGNED_INT, nullptr);
    mPathVisShader.unbind();
}

