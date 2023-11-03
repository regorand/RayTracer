#pragma once

#include "GL/glew.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"
#include "utils/UniformDistribution.h"

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

class PathTracer {
private:
    const unsigned int TARGET_BUFFER_TARGET = 0;

    const unsigned int POS_BUFFER_TARGET = 1;
    const unsigned int DIR_BUFFER_TARGET = 2;
    const unsigned int MULT_BUFFER_TARGET = 3;

    const GLenum POS_BUFFER_FORMAT = GL_RGBA32F;
    const GLenum DIR_BUFFER_FORMAT = GL_RGBA32F;
    const GLenum MULT_BUFFER_FORMAT = GL_RGBA32F;

    int mWidth;
    int mHeight;

    // TODO get TextureBuffers working ? -> More efficient creation
    Texture mPosBuffer;
    Texture mDirBuffer;
    Texture mMultiplierBuffer;

    ComputeShader mNewPathShader;
    ComputeShader mBounceShader;
    // TODO want a terminate path shader ?
    ComputeShader mClearTargetShader;

    PathTraceSettings mPathTraceSettings;

    UniformDistribution dist;

    int mSppCount = 0;

    RenderStatus mRenderStatus;

    Texture *mCurrentTarget;

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

    void reloadShaders();

    void clearTarget();

    void buildImGuiWindow();
    void buildImGuiProgressBar(glm::vec2 displaySize);
private:
    bool renderFenceSignalled();

    void bindBounce();
    void unbindBounce();

    void newPath();
    void renderSingleBounce();


};