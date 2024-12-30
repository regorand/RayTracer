#pragma once

#include <memory>
#include "gui/ImGuiSidebarTab.h"
#include "scene/Scene.h"
#include "glWrappers/Texture.h"
#include "utils/RandomEngine.h"
#include "RenderStatistics.h"


class MCIntegrator : public ImGuiSidebarTab {
    const unsigned int TARGET_TEXTURE_TARGET = 0;
    constexpr static int WORK_GROUP_SIZE_1_DIM = 8; // Is set in shader, hardcoded here

    std::vector<ShaderProgram> shaders;
    int activeShader = 0;

    std::shared_ptr<Scene> mScene;
    Texture *mCurrentTarget;

    GLsync fence = nullptr;

    RandomEngine dist;

    bool dirty = false;

    bool doBrdf = true;
    bool doNee = true;

    int maxBounces = 4;
    bool doRR = false;
    float alphaRR = 0.8;

    bool showStats = true;

    RenderStatistics stats;

    GLuint timerQuery = -1;
    bool queryResultPending = false;
    bool disableStats = false;

public:
    MCIntegrator();
    ~MCIntegrator() {
        glDeleteQueries(1, &timerQuery);
    }

    void renderSample();

    void setTarget(Texture *target);
    Texture *getTarget();

    void setScene(std::shared_ptr<Scene> newScene);
    std::shared_ptr<Scene> getScene();

    bool currentlyWorking();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

    RenderStatistics getRenderStatistics();

    void resetRenderStatistics();

    void handleQueryResult();

    bool collectDirty();
private:

    void bindScene();
    void unbindScene();

    void bindUniforms();

    void reloadShaders();

    bool queryAvailable();
    void setupQuery();
    void startQuery();
    void endQuery();
};