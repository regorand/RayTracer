#ifndef RAYTRACER_APPLICATION_H
#define RAYTRACER_APPLICATION_H

#include "utils/glewIncludeUtils.h"
#include "renderers/BufferRenderer.h"
#include "gui/ImguiUtils.h"
#include "renderers/PostProcessor.h"
#include "userInput/KeyStateManager.h"
#include "renderers/Rasterizer.h"
#include "userInput/CameraManipulator.h"
#include "dataStructures/DataStore.h"
#include "dataStructures/Events.h"
#include <GLFW/glfw3.h>

class Application {
private:
    GLFWwindow *window;

    int displayWidth, displayHeight;

    DataStore settings;
    std::vector<Event> events;

    KeyStateManager *keyStateManager = KeyStateManager::getInstance();
    CameraManipulator cameraManipulator;

    bool dirty = false;
public:
    Application();

    int run();

private:
    void buildImguiControlSideBar(glm::vec2 displaySize, std::vector<ImGuiSidebarTab*> &sideBarTabs, std::map<int, Texture> &textureStore);
};


#endif //RAYTRACER_APPLICATION_H
