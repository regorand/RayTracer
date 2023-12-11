#ifndef RAYTRACER_APPLICATION_H
#define RAYTRACER_APPLICATION_H

#include "utils/glewIncludeUtils.h"
#include "renderers/PathTracer.h"
#include "renderers/BufferRenderer.h"
#include "ImguiUtils.h"
#include "renderers/PostProcessor.h"
#include "userInput/KeyStateManager.h"
#include "renderers/Rasterizer.h"
#include "userInput/CameraManipulator.h"
#include <GLFW/glfw3.h>

typedef struct RenderSettings_s {
    bool alwaysRenderNew;

    RenderSettings_s() {
        this->alwaysRenderNew = false;
    }
} RenderSettings;

static bool movementKeys[7];

class Application {
private:
    GLFWwindow *window;

    GLsync currentSync = nullptr;
    bool useSync = false;

    int displayWidth, displayHeight;

    KeyStateManager *keyStateManager = KeyStateManager::getInstance();
    CameraManipulator cameraManipulator;

public:
    Application();

    int run();

private:
    void
    buildImguiControlSideBar(glm::vec2 displaySize, ImGuiMenuSettings &menuSettings, PathTracer &pt, BufferRenderer &br,
                             Rasterizer &rasterizer, PostProcSettings &postProcSettings);

    void buildImguiCameraControls(PathTracer &pt,Rasterizer &rasterizer);
};


#endif //RAYTRACER_APPLICATION_H
