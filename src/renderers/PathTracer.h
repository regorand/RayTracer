#pragma once

#include <chrono>
#include "GL/glew.h"
#include "glWrappers/Texture.h"
#include "glWrappers/shaders/ComputeShader.h"
#include "utils/RandomEngine.h"
#include "scene/OldScene.h"
#include "glWrappers/TextureBuffer.h"
#include "glWrappers/ShaderStorageBuffer.h"
#include "dataStructures/DataStore.h"
#include "gui/ImGuiSidebarTab.h"
#include "glWrappers/shaders/ShaderProgram.h"

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

} RenderStatus;

typedef struct ptBuffers_s {
    ShaderStorageBuffer posBuffer;
    ShaderStorageBuffer dirBuffer;
    ShaderStorageBuffer multiplierBuffer;

    ptBuffers_s(unsigned int size, GLenum usage)
            : posBuffer(size, usage), dirBuffer(size, usage), multiplierBuffer(size, usage) {}

    static std::vector<std::string> getAsList() {
        return {"Pos buffer", "Dir buffer", "Color buffer"};
    }

    ShaderStorageBuffer &getByIndex(int index) {
        switch (index) {
            case 0: return posBuffer;
            case 1: return dirBuffer;
            case 2: return multiplierBuffer;
            default: return posBuffer;
        }
    }
} PathTracerBuffers;

using namespace std::chrono;

class PathTracer : public ImGuiSidebarTab {
private:
    const unsigned int TARGET_TEXTURE_TARGET = 0;

    int mWidth;
    int mHeight;

    PathTracerBuffers buffers;

    ShaderProgram mNewPathShader;
    ShaderProgram mBounceShader;
    ShaderProgram mClearTargetShader;
    // TODO want a terminate path shader ?

    PathTraceSettings mPathTraceSettings;

    RandomEngine dist;

    int mSppCount = 0;

    RenderStatus mRenderStatus;

    Texture *mCurrentTarget;

    OldScene mScene;

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

    void setScene(OldScene &scene);

    void reloadShaders();

    void clearTarget();

    void buildImGuiWindow(DataStore &settings);

    void buildImGuiProgressBar(glm::vec2 displaySize);

    void enablePathRecording(bool enable);

    bool hasRecordedPath();

    Path &getRecordedPath();

    void setRecordingCoords(glm::vec2 coords);

    OldScene &getSceneRef();

    PathTracerBuffers &getBuffers();

    virtual void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

private:
    bool renderFenceSignalled();

    void bindBounce();

    void unbindBounce();

    void newPath();

    void renderSingleBounce();

    void recordPathVertex();

};