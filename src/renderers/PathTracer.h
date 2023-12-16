#pragma once

#include <chrono>
#include "GL/glew.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"
#include "utils/UniformDistribution.h"
#include "scene/Scene.h"
#include "glWrappers/TextureBuffer.h"
#include "glWrappers/ShaderStorageBuffer.h"

typedef struct path_s {
    std::vector<glm::vec3> vertices;
    int pathLength = 0;
} Path;

typedef struct PathSettings_s {
    // Path length Params
    bool russianRoulette;
    int constantPathLength;
    float alphaRR; // survival rate for russian roulette path length

    PathSettings_s() {
        russianRoulette = true;
        constantPathLength = 1;
        alphaRR = 0.75;
    }
} PathSettings;

typedef struct PathTraceSettings_s {
    int spp;
    PathSettings pathSettings;

    PathTraceSettings_s(int spp = 16) {
        this->spp = spp;
        this->pathSettings = PathSettings();
    }
} PathTraceSettings;

enum class RenderState {
    IDLE,           // no rendering
    BUSY,           // currently rendering, waiting for bounce computation to finish
    DUE_BOUNCE,     // currently rendering, in the middle of a path: next compute shader will compute bounce
    DUE_NEW_PATH    // currently rendering, just finished a path: next compute shader will generate new path
};

typedef struct RenderStatus_s {
    RenderState state;

    GLsync sync;

    int targetSpp;
    int currentSpp;

    RenderStatus_s() {
        reset();
        targetSpp = 1;
    }

    void reset() {
        state = RenderState::IDLE;

        sync = nullptr;
        currentSpp = 1;
    }

} RenderStatus ;

using namespace std::chrono;

class PathTracer {
private:
    const unsigned int TARGET_TEXTURE_TARGET = 0;

    int mWidth;
    int mHeight;

    ShaderStorageBuffer posBuffer;
    ShaderStorageBuffer dirBuffer;
    ShaderStorageBuffer multiplierBuffer;

    ComputeShader mNewPathShader;
    ComputeShader mBounceShader;
    // TODO want a terminate path shader ?
    ComputeShader mClearTargetShader;

    PathTraceSettings mPathTraceSettings;

    UniformDistribution dist;

    int mSppCount = 0;

    RenderStatus mRenderStatus;

    Texture *mCurrentTarget;

    Scene mScene;

    milliseconds renderStart;

    Path recordedPath;
    glm::vec2 recordingCoords;
    bool enableRecording;

public:
    PathTracer(int width, int height);

    void startRender();
    RenderState doRenderStep();

    void setPathTraceSettings(PathTraceSettings pathTraceSettings);
    PathTraceSettings getPathTraceSettings();

    RenderStatus getRenderStatus();
    RenderState getRenderState();

    void cancelRender();

    void setTargetp(Texture *target);

    void setScene(Scene &scene);

    void reloadShaders();

    void clearTarget();

    void buildImGuiWindow();
    void buildImGuiProgressBar(glm::vec2 displaySize);

    void enablePathRecording(bool enable);
    bool hasRecordedPath();
    Path &getRecordedPath();
    void setRecordingCoords(glm::vec2 coords);

    Scene &getSceneRef();

private:
    bool renderFenceSignalled();

    void bindBounce();
    void unbindBounce();

    void newPath();
    void renderSingleBounce();

    void recordPathVertex();

};