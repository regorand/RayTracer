#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "gui/ImguiUtils.h"
#include "renderers/PostProcessor.h"
#include "renderers/Rasterizer.h"
#include "dataStructures/SettingIndex.h"
#include "gui/ImGuiSidebarTab.h"
#include "scene/camera/CameraController.h"
#include "glWrappers/TextureFactory.h"
#include "glWrappers/FrameBufferObject.h"
#include "dataStructures/TextureNames.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/Scene.h"
#include "renderers/MCIntegrator.h"
#include "scene/camera/manipulators/OrbitManipulator.h"
#include <imgui.h>
#include <filesystem>


Application::Application() {
}


int Application::run() {
    int initRes = init();
    if (initRes) return initRes;

    while (!glfwWindowShouldClose(window)) {
        renderLoop();
    }

    clear();
    return 0;
}


int Application::init() {
    initGLFW();
    initImGui();
    initOpenGL();

    setupAppTools();

    loadScene("res/models/cornell_box_boxes.gltf");
    return 0;
}

int Application::renderLoop() {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glViewport(0, 0, displayWidth, displayHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    buildImgui();

    ImGui::GetMouseDragDelta();

    auto &activeScene = scenes[activeSceneIndex];

    dirty |= cameraManipulators[mActiveManipulatorIndex]->updateCamera(activeScene->getActiveCamera(), glm::vec2(displayWidth, displayHeight));

    if (settings.getOrElse(SHOW_PATHTRACE_OUTPUT, false)) {
        dirty |= !accumulate;
        if (dirty) {
            textureStore.at(RENDER_TARGET).clear();
            dirty = false;
        }

        mcIntegrator->renderSample();

        // Idle waiting not ideal, but with current implementation need to wait for Integrator to finish
        // so single sample image is visible while moving the camera
        while (mcIntegrator->currentlyWorking()) {}

        postProcessor->applyGammaAndTonemap(textureStore.at(RENDER_TARGET), textureStore.at(DISPLAY_BUFFER), settings);
        br->render(&textureStore.at(DISPLAY_BUFFER));
    } else {
        rasterizer->render(displayWidth, displayHeight);
    }

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

    wasResized = false;

    return 0;
}

int Application::clear() {
    for (auto &texture: textureStore) {
        texture.second.destroy();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


int Application::initGLFW() {
    if (!glfwInit()) {
        return 1;
    }

    const std::string application_name = "RayTracer";

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, application_name.c_str(), nullptr, nullptr);


    if (window == nullptr)
        return 1;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);


    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeCallback(window, &Application::windowResizeCallback);


    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    return 0;
}

int Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // io.AddInputCharacter(ImGuiKey_Backspace);
    // io.KeysDown[ImGuiKey_Backspace] = true;
    // io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    // io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;

    prevMousePos = ImGui::GetMousePos();

    ImGui::StyleColorsDark();

    return 0;
}

int Application::initOpenGL() {
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Glew init failed");
    }


    int major;
    int minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);

    registerGLCallBack();
    return 0;
}

int Application::setupAppTools() {
    strncpy_s(pathInputBuf, DEFAULT_MODEL_DIR.c_str(), std::min(DEFAULT_MODEL_DIR.size(), PATH_INPUT_BUF_SIZE));
    setupTextures();

    rasterizer = std::make_shared<Rasterizer>();

    br = std::make_shared<BufferRenderer>();
    postProcessor = std::make_shared<PostProcessor>(displayWidth, displayHeight);

    mcIntegrator = std::make_shared<MCIntegrator>();
    mcIntegrator->setTarget(&textureStore.at(RENDER_TARGET));

    /*
    textureStore.insert({POS_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({NORMAL_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({ALBEDO_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    textureStore.insert({DEPTH_BUFFER, createDepthBuffer(displayWidth, displayHeight)});
    */

    settings.store(SHOW_STATS_WINDOW, false);

    settings.store(GAMMA_CORRECT, true);
    settings.store(SHOW_DENOISE, false);

    // CameraController cameraController(pathTracer, rasterizer);

    settings.store(SHOW_CONTROL_PANEL, true);

    cameraManipulators.push_back(std::make_shared<OrbitManipulator>());
    cameraManipulators.push_back(std::make_shared<FlightManipulator>());
    mActiveManipulatorIndex = 0;

    sideBarTabs.push_back(mcIntegrator.get());
    // sideBarTabs.push_back(scene.get());
    sideBarTabs.push_back(rasterizer.get());
    // sideBarTabs.push_back(&cameraController);
    sideBarTabs.push_back(postProcessor.get());
    sideBarTabs.push_back(br.get());

    return 0;
}


int Application::setupTextures() {
    textureStore.clear();

    textureStore.insert({RENDER_TARGET, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({DISPLAY_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    return 0;
}

void Application::buildImguiControlSideBar() {
    float sideBarWidth = static_cast<float>(displayWidth) / 4.0f;

    glm::vec2 menuBarSize = settings.get<glm::vec2>(MENU_BAR_SIZE).value_or(glm::vec2(0));
    float xPos = displayWidth - sideBarWidth;

    ImGuiCond_ flags = wasResized ? ImGuiCond_Always : ImGuiCond_Once;

    ImGui::SetNextWindowPos(ImVec2(xPos, menuBarSize.y), flags);
    ImGui::SetNextWindowSize(ImVec2(sideBarWidth, static_cast<float>(displayHeight)), flags);

    ImGui::Begin("Control Panel", settings.
            getPointer<bool>(SHOW_CONTROL_PANEL), ImGuiWindowFlags_NoMove);

    ImGui::Checkbox("Show Pathtrace output", settings.
            getPointer<bool>(SHOW_PATHTRACE_OUTPUT)
    );

    if (ImGui::Button("reset Texture")) {
        textureStore.at(RENDER_TARGET).clear();
    }

    ImGui::Checkbox("Accumulate", &accumulate);

    sideBarTabs.push_back(mcIntegrator.get());
    // sideBarTabs.push_back(scene.get());
    sideBarTabs.push_back(rasterizer.get());
    // sideBarTabs.push_back(&cameraController);
    sideBarTabs.push_back(postProcessor.get());
    sideBarTabs.push_back(br.get());

    startIndent();

    mcIntegrator->buildImguiTab(settings, events);
    endAndNewIndent();

    if (ImGui::CollapsingHeader("Scenes")) {
        for (int i = 0; i < scenes.size(); i++) {
            auto &scene = scenes[i];
            const auto &name = scene->getName();
            if (ImGui::Button(name.c_str())) {
                if (activeSceneIndex != i) {
                    activeSceneIndex = i;
                    updateActiveScene();
                    dirty = true;
                }
            }
        }
    }
    endAndNewIndent();

    if (ImGui::CollapsingHeader("Camera Manipulators")) {
        mActiveManipulatorIndex = glm::min(glm::max(mActiveManipulatorIndex, 0), (int) cameraManipulators.size() - 1);

        auto selected = cameraManipulators[mActiveManipulatorIndex];
        ImGui::Text("Camera Manipulator");
        if (ImGui::BeginCombo("##chhoseManipulator", cameraManipulators[mActiveManipulatorIndex]->getDisplayName().c_str())) {
            for (int i = 0; i < cameraManipulators.size(); i++) {
                bool is_selected = (selected->getDisplayName() == cameraManipulators[i]->getDisplayName()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(cameraManipulators[i]->getDisplayName().c_str(), is_selected)) {
                    selected = cameraManipulators[i];
                    mActiveManipulatorIndex = i;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)

            }

            ImGui::EndCombo();
        }
        ImGui::Separator();
        cameraManipulators[mActiveManipulatorIndex]->buildImgui(scenes[activeSceneIndex]->getActiveCamera());
    }
    endAndNewIndent();

    postProcessor->buildImguiTab(settings, events);
    endAndNewIndent();

    br->buildImguiTab(settings, events);
    endAndNewIndent();

    rasterizer->buildImguiTab(settings, events);

    endIndent();


    ImGui::End();

}

void Application::updateActiveScene() {
    auto &activeScene = scenes[activeSceneIndex];
    mcIntegrator->setScene(activeScene);
    rasterizer->setScene(activeScene);
}

void Application::buildImgui() {
    buildImguiMenu();

    if (settings.get<bool>(SHOW_FILE_PATH_DIALOG).value_or(false)) {
        buildFileOpenDialog();
    }

    if (settings.get<bool>(SHOW_STATS_WINDOW).value_or(false)) {
        buildImguiAppStats(settings);
    }

    if (settings.getOrElse<bool>(SHOW_CONTROL_PANEL, false)) {
        buildImguiControlSideBar();
    }
}

void Application::buildImguiMenu() {
    if (ImGui::BeginMainMenuBar()) {
        settings.store(SettingIndex::MENU_BAR_SIZE, ImVec2ToGlmVec2(ImGui::GetWindowSize()));
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("TODO: Load Scene")) {
                settings.store(SHOW_FILE_PATH_DIALOG, true);
            }
            if (ImGui::MenuItem("TODO: Save Image"));
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows")) {
            // TODO change message dynamically

            if (ImGui::MenuItem("App Statistics")) {
                settings.store(SHOW_STATS_WINDOW, !settings.getOrElse(SHOW_STATS_WINDOW, true));
            }

            if (ImGui::MenuItem("Control Panel")) {
                settings.store(SHOW_CONTROL_PANEL, !settings.getOrElse(SHOW_CONTROL_PANEL, true));
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

}

void Application::loadScene(const std::string &path, bool activateOnLoad) {
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    bool success = scene->loadFile(path);
    if (!success) return;
    scenes.push_back(scene);

    if (activateOnLoad) {
        activeSceneIndex = scenes.size() - 1;
        updateActiveScene();
        dirty = true;
    }
}

void Application::windowResizeCallback(GLFWwindow *window, int width, int height) {
    auto *app = (Application *) glfwGetWindowUserPointer(window);

    app->handleResize(window, width, height);
}

void Application::handleResize(GLFWwindow *window, int width, int height) {
    windowWidth = width;
    windowHeight = height;

    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

    postProcessor->resize(displayWidth, displayHeight);

    setupTextures();
    mcIntegrator->setTarget(&textureStore.at(RENDER_TARGET));

    wasResized = true;
}

void Application::buildFileOpenDialog() {
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(displayWidth) / 2, static_cast<float>(displayHeight) / 2),
                            0 ,
                            ImVec2(0.5f, 0.5f));

    float width = std::min(500.0f, static_cast<float>(displayWidth) * 0.8f);
    ImGui::SetNextWindowSize(ImVec2(width, 150));

    ImGui::Begin("Load new File", settings.getPointer<bool>(SHOW_FILE_PATH_DIALOG));

    ImGui::InputText("##Scene Path", pathInputBuf, PATH_INPUT_BUF_SIZE);
    if (ImGui::Button("Load")) {
        std::string path_str(pathInputBuf);
        std::filesystem::path path(path_str);
        if (!exists(path)) {
            loadErrorString = "File could not be found";
            settings.store(FILE_PATH_SHOW_ERROR, true);
        }

        if (path.extension() != ".gltf") {
            loadErrorString = "Can only load gltf files currently";
            settings.store(FILE_PATH_SHOW_ERROR, true);
        }

        if (!settings.getOrElse(FILE_PATH_SHOW_ERROR, false)) {
            strncpy_s(pathInputBuf, DEFAULT_MODEL_DIR.c_str(), std::min(DEFAULT_MODEL_DIR.size(), PATH_INPUT_BUF_SIZE));

            settings.store(SHOW_FILE_PATH_DIALOG, false);
            settings.store(FILE_PATH_SHOW_ERROR, false);
            loadErrorString.clear();

            loadScene(path.string());
        }
    }
    if (settings.getOrElse(FILE_PATH_SHOW_ERROR, false)) {
        ImGui::Text(loadErrorString.c_str());
    }

    ImGui::End();
}

