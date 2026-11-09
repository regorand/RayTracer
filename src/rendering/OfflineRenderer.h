#pragma once

#include <memory>
#include "scene/Scene.h"
#include "glWrappers/Texture.h"
#include "RenderStatistics.h"
#include "utils/RandomEngine.h"

class OfflineRenderer : public ImGuiSidebarTab {

protected:
    const unsigned int TARGET_TEXTURE_TARGET = 0;
    constexpr static int WORK_GROUP_SIZE_1_DIM = 8; // Is set in shader, hardcoded here

    std::vector<ShaderProgram> shaders;
    int activeShader = 0;

    std::shared_ptr<Scene> mScene;
    Texture *mCurrentTarget;

    GLsync fence = nullptr;

    RandomEngine dist;

    bool dirty = false;

    bool showStats = true;

    RenderStatistics stats;

    GLuint timerQuery = -1;
    bool queryResultPending = false;
    bool disableStats = false;


public:
    OfflineRenderer();
    virtual ~OfflineRenderer();

    virtual void renderSample() = 0;

    // void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
    void setTarget(Texture *target);

    Texture *getTarget();
    void setScene(std::shared_ptr<Scene> newScene);

    std::shared_ptr<Scene> getScene();

    bool currentlyWorking();

    RenderStatistics getRenderStatistics();

    void resetRenderStatistics();

    void handleQueryResult();

    bool collectDirty();

    void reloadShaders();
protected:
    virtual void bindUniforms() = 0;

    void bindScene();

    void unbindScene();

    bool queryAvailable();
    void setupQuery();
    void cleanQuery();
    void startQuery();
    void endQuery();
};
