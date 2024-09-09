#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "gui/ImguiUtils.h"
#include "renderers/PathTracer.h"
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
#include "scene/SceneImporter.h"
#include "renderers/MCIntegrator.h"
#include <imgui.h>

Application::Application() {
}


int Application::run() {

    if (!glfwInit()) {
        return 1;
    }

    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    const int width = 800;
    const int height = 500;

    const std::string application_name = "RayTracer";

    window = glfwCreateWindow(width, height, application_name.c_str(), nullptr, nullptr);
    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

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

    keyStateManager->registerGLFW(window);

    registerGLCallBack();

    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    // scene->loadFile("res/models/cornell_box_boxes.gltf");
    scene->loadFile("res/models/cornell_box.gltf");

    Rasterizer rasterizer;
    rasterizer.setScene(scene);

    BufferRenderer br;

    std::map<int, Texture> textureStore;
    textureStore.insert({RENDER_TARGET, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({DISPLAY_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    textureStore.insert({POS_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({NORMAL_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({ALBEDO_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    textureStore.insert({DEPTH_BUFFER, createDepthBuffer(displayWidth, displayHeight)});

    PostProcessor postProcessor(displayWidth, displayHeight);

    FrameBufferObject fbo;
    fbo.addTextureAttachment(textureStore.at(POS_BUFFER), GL_COLOR_ATTACHMENT0);
    fbo.addTextureAttachment(textureStore.at(NORMAL_BUFFER), GL_COLOR_ATTACHMENT1);
    fbo.addTextureAttachment(textureStore.at(ALBEDO_BUFFER), GL_COLOR_ATTACHMENT2);
    fbo.addTextureAttachment(textureStore.at(DEPTH_BUFFER), GL_DEPTH_ATTACHMENT);

    PathTracer pathTracer(displayWidth, displayHeight);
    pathTracer.setScene(scene);

    pathTracer.setTargetp(&textureStore.at(RENDER_TARGET));
    pathTracer.clearTarget();

    pathTracer.enablePathRecording(false);

    keyStateManager->getMouseAndGetDiff(window);

    settings.store(SHOW_STATS_WINDOW, false);

    settings.store(GAMMA_CORRECT, true);

    // CameraController cameraController(pathTracer, rasterizer);

    settings.store(SHOW_CONTROL_PANEL, true);

    MCIntegrator mcIntegrator;
    mcIntegrator.setScene(scene);
    mcIntegrator.setTarget(&textureStore.at(RENDER_TARGET));

    std::vector<ImGuiSidebarTab *> sideBarTabs;
    sideBarTabs.push_back(&pathTracer);
    sideBarTabs.push_back(&mcIntegrator);
    sideBarTabs.push_back(scene.get());
    sideBarTabs.push_back(&rasterizer);
    // sideBarTabs.push_back(&cameraController);
    sideBarTabs.push_back(&postProcessor);
    sideBarTabs.push_back(&br);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        RenderState state = pathTracer.getRenderState();
        if (state != RenderState::IDLE) {
            pathTracer.doRenderStep();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        buildImguiMenu(settings);

        if (settings.get<bool>(SHOW_STATS_WINDOW).value_or(false)) {
            buildImguiAppStats(settings);
        }

        if (settings.getOrElse<bool>(SHOW_CONTROL_PANEL, false)) {
            buildImguiControlSideBar(glm::vec2(displayWidth, displayHeight), sideBarTabs);
        }

        if (state != RenderState::IDLE) pathTracer.buildImGuiProgressBar(glm::vec2(displayWidth, displayHeight));


        glm::vec2 diff = keyStateManager->getMouseAndGetDiff(window);
        dirty = cameraManipulator.updateCamera(scene->getActiveCamera(), keyStateManager, diff,
                                               glm::vec2(displayWidth, displayHeight));

        if (settings.getOrElse(SHOW_PATHTRACE_OUTPUT, false)) {
            if (dirty) {
                pathTracer.clearTarget();
            }

            mcIntegrator.renderSample();

            // Idle waiting not ideal, but with current implementation need to wait for Integrator to finish
            // so single sample image is visible while moving the camera
            while (mcIntegrator.currentlyWorking()) { }
            br.render(&textureStore.at(RENDER_TARGET));
        } else {
            rasterizer.render();
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    /*
    for (auto &texture : textureStore) {
        texture.second.destroy();
    }
     */

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


void Application::buildImguiControlSideBar(glm::vec2 displaySize, std::vector<ImGuiSidebarTab*> &sideBarTabs) {

    float windowWidth = displaySize.x / 6.0f;

    glm::vec2 menuBarSize = settings.get<glm::vec2>(MENU_BAR_SIZE).value_or(glm::vec2(0));

    ImGui::SetNextWindowPos(
            ImVec2(displaySize.x - windowWidth, menuBarSize.y),
            ImGuiCond_Once
    );
    ImGui::SetNextWindowSize(ImVec2(windowWidth, displaySize.y), ImGuiCond_Once
    );
    ImGui::Begin("Control Panel", settings.
                         getPointer<bool>(SHOW_CONTROL_PANEL),
                 ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoMove);

// ImGui::Checkbox("Show Pathtrace output", &menuSettings.viewPathTraceOutput);
    ImGui::Checkbox("Show Pathtrace output", settings.
            getPointer<bool>(SHOW_PATHTRACE_OUTPUT)
    );

    startIndent();
    for (auto sideBarTab: sideBarTabs) {
        sideBarTab->buildImguiTab(settings, events);
        endAndNewIndent();
    }
    endIndent();


    ImGui::End();

}

void Application::buildImguiCameraControls(PathTracer &pt, Rasterizer &rasterizer) {
    if (ImGui::CollapsingHeader("Camera Controls")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reset debug camera", buttonSize)) {
            // TODO
            //rasterizer.getSceneRef().setCamera(pt.getSceneRef().getCamera());
        }

        if (ImGui::Button("Pathtrace camera to debug camera", buttonSize)) {
            // TODO
            // pt.getSceneRef().setCamera(rasterizer.getSceneRef().getCamera());
        }

    }
}