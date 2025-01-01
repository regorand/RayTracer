#ifndef RAYTRACER_APPLICATION_H
#define RAYTRACER_APPLICATION_H

#include "utils/glewIncludeUtils.h"
#include "renderers/BufferRenderer.h"
#include "gui/ImguiUtils.h"
#include "renderers/PostProcessor.h"
#include "userInput/KeyStateManager.h"
#include "renderers/Rasterizer.h"
#include "scene/camera/manipulators/FlightManipulator.h"
#include "dataStructures/DataStore.h"
#include "dataStructures/Events.h"
#include "renderers/MCIntegrator.h"
#include "renderers/BVHRenderer.h"
#include <GLFW/glfw3.h>

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 500

inline std::string DEFAULT_MODEL_DIR = "res/models/";

class Application {
private:
    GLFWwindow *window;

    int windowWidth, windowHeight;
    int displayWidth, displayHeight;

    DataStore settings;
    std::vector<Event> events;

    std::vector<std::shared_ptr<Scene>> scenes;
    int activeSceneIndex = 0;

    std::map<int, Texture> textureStore;

    std::shared_ptr<Rasterizer> rasterizer;
    std::shared_ptr<BufferRenderer> br;
    std::shared_ptr<PostProcessor> postProcessor;


    std::shared_ptr<MCIntegrator> mcIntegrator;
    std::shared_ptr<BVHRenderer> bvhRenderer;

    std::vector<std::shared_ptr<CameraManipulator>> cameraManipulators;
    int mActiveManipulatorIndex;


    // TODO better to use a dropdown or something once I am done testing
    bool visBVH = false;

    bool accumulate = true;
    bool render = true;
    bool dirty = false;
    bool wasResized = false;

    static constexpr size_t PATH_INPUT_BUF_SIZE = 128;
    char pathInputBuf[PATH_INPUT_BUF_SIZE]{};
    std::string loadErrorString;

    ImVec2 prevMousePos;
public:
    Application();

    int run();

private:
    int init();
    int renderLoop();
    int clear();

    int initGLFW();
    int initImGui();
    int initOpenGL();
    int setupAppTools();
    int setupTextures();

    void loadScene(const std::string &path, bool activateOnLoad = true);
    void updateActiveScene();

    void buildImgui();
    void buildImguiMenu();
    void buildFileOpenDialog();

    void buildImguiControlSideBar();

    static void windowResizeCallback(GLFWwindow *window, int width, int height);
    void handleResize(GLFWwindow *window, int width, int height);
};


#endif //RAYTRACER_APPLICATION_H
