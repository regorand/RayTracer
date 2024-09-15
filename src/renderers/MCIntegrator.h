#pragma once

#include <memory>
#include "gui/ImGuiSidebarTab.h"
#include "scene/Scene.h"
#include "glWrappers/Texture.h"
#include "utils/RandomEngine.h"


class MCIntegrator : public ImGuiSidebarTab {
    const unsigned int TARGET_TEXTURE_TARGET = 0;
    constexpr static int WORK_GROUP_SIZE_1_DIM = 8; // Is set in shader, hardcoded here

    ShaderProgram mcIntegrator;

    std::shared_ptr<Scene> mScene;
    Texture *mCurrentTarget;

    GLsync fence = nullptr;

    RandomEngine dist;

    bool doBrdf = true;
    bool doNee = false;

public:
    MCIntegrator();

    void renderSample();

    void setTarget(Texture *target);
    Texture *getTarget();

    void setScene(std::shared_ptr<Scene> newScene);
    std::shared_ptr<Scene> getScene();

    bool currentlyWorking();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
private:

    void bindScene();
    void unbindScene();

    void bindUniforms();

    void reloadShaders();
};
