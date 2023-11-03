#ifndef RAYTRACER_APPLICATION_H
#define RAYTRACER_APPLICATION_H

#include "utils/glewIncludeUtils.h"
#include "renderers/PathTracer.h"
#include "renderers/BufferRenderer.h"
#include "ImguiUtils.h"
#include <GLFW/glfw3.h>

typedef struct RenderSettings_s {
    bool alwaysRenderNew;

    RenderSettings_s () {
        this->alwaysRenderNew = false;
    }
} RenderSettings;

class Application {
private:
    GLFWwindow* window;

    GLsync currentSync = nullptr;
    bool useSync = false;

public:
    Application();

    int run();

private:
    void buildImguiControlSideBar(glm::vec2 displaySize, ImGuiMenuSettings menuSettings, PathTracer &pt, BufferRenderer &br);

};


#endif //RAYTRACER_APPLICATION_H
