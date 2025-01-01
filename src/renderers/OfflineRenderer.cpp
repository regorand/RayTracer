#include "OfflineRenderer.h"
#include "deviceShared/BufferBindings.h"

OfflineRenderer::OfflineRenderer() {
    setupQuery();
}

OfflineRenderer::~OfflineRenderer() {
    cleanQuery();
}

bool OfflineRenderer::currentlyWorking() {
    if (fence == nullptr) {
        return false;
    }

    GLint signal = 0;
    glGetSynciv(fence, GL_SYNC_STATUS, 1, nullptr, &signal);

    if (signal == GL_SIGNALED) {
        glDeleteSync(fence);
        fence = nullptr;
        return false;
    }

    return true;
}

void OfflineRenderer::bindScene() {
    GPUSceneBuffers &gpuBuffers = mScene->getGPUBuffers();
    gpuBuffers.triangleBuffer.bindToTarget(TRIANGLE_BUFFER_BINDING);
    gpuBuffers.objectBuffer.bindToTarget(OBJECT_BUFFER_BINDING);
    gpuBuffers.meshBuffer.bindToTarget(MESH_BUFFER_BINDING);
    gpuBuffers.materialBuffer.bindToTarget(MATERIAL_BUFFER_BINDING);
    gpuBuffers.emissiveBuffer.bindToTarget(EMISSIVE_BUFFER_BINDING);
    gpuBuffers.transformationBuffer.bindToTarget(TRANSFORM_BUFFER_BINDING);
    gpuBuffers.bvhBuffer.bindToTarget(BVH_NODES_BUFFER_BINDING);
}

void OfflineRenderer::unbindScene() {
    GPUSceneBuffers &gpuBuffers = mScene->getGPUBuffers();
    gpuBuffers.triangleBuffer.unbind();
    gpuBuffers.objectBuffer.unbind();
    gpuBuffers.meshBuffer.unbind();
    gpuBuffers.materialBuffer.unbind();
    gpuBuffers.emissiveBuffer.unbind();
    gpuBuffers.transformationBuffer.unbind();
    gpuBuffers.bvhBuffer.unbind();
}

void OfflineRenderer::setTarget(Texture *target) {
    mCurrentTarget = target;
}

Texture *OfflineRenderer::getTarget() {
    return mCurrentTarget;
}

void OfflineRenderer::setScene(std::shared_ptr<Scene> newScene) {
    mScene = newScene;
}

std::shared_ptr<Scene> OfflineRenderer::getScene() {
    return mScene;
}

void OfflineRenderer::reloadShaders() {
    for (auto &shader: shaders) {
        shader.reloadShader();
    }
}

RenderStatistics OfflineRenderer::getRenderStatistics() {
    return stats;
}


void OfflineRenderer::resetRenderStatistics() {
    stats.reset();
}

bool OfflineRenderer::collectDirty() {
    bool tmp = dirty;
    dirty = false;
    return tmp;
}

void OfflineRenderer::setupQuery() {
    glGenQueries(1, &timerQuery);
}

void OfflineRenderer::handleQueryResult() {
    if (!queryResultPending || disableStats) return;
    GLint64 result;
    glGetQueryObjecti64v(timerQuery, GL_QUERY_RESULT, &result);
    double msResult = (double) result / 1e6;
    stats.lastMsTime = msResult;
    stats.summedTime += msResult;
    stats.movingAverage = stats.alpha * msResult + (1 - stats.alpha) * stats.movingAverage;
    stats.timingSamplesCollected++;
    queryResultPending = false;
}

void OfflineRenderer::startQuery() {
    if (disableStats) return;
    glFlush();
    glBeginQuery(GL_TIME_ELAPSED, timerQuery);
}

void OfflineRenderer::cleanQuery() {
    glDeleteQueries(1, &timerQuery);
}

void OfflineRenderer::endQuery() {
    if (disableStats) return;
    glFlush();
    glEndQuery(GL_TIME_ELAPSED);
    queryResultPending = true;
}

bool OfflineRenderer::queryAvailable() {
    if (disableStats) return false;
    GLint64 result;
    glGetQueryObjecti64v(timerQuery, GL_QUERY_RESULT_AVAILABLE, &result);
    return result == GL_TRUE;
}





