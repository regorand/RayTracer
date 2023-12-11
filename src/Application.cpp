#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "ImguiUtils.h"
#include "renderers/PathTracer.h"
#include "scene/Scene.h"
#include "utils/SceneCreateUtils.h"
#include "renderers/PostProcessor.h"
#include "renderers/Rasterizer.h"
#include <imgui.h>

Application::Application() {
}


int Application::run() {

    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

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

    BufferRenderer br;

    HostBuffer buffer(displayWidth, displayHeight);

    Texture renderTarget(&buffer);
    renderTarget.setAccessType(GL_READ_WRITE);

    Texture displayBuffer(&buffer);
    displayBuffer.setAccessType(GL_READ_WRITE);

    Scene scene;
    Camera camera(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1));
    camera.setAspect((float) displayWidth / (float) displayHeight);
    scene.setCamera(camera);

    std::vector<MeshObject> objects;
    createCornellBox(objects);
    createSampleBoxes(objects);

    for (MeshObject &object: objects) {
        scene.addMeshObject(object);
    }
    scene.createGPUBuffers();
    scene.buildRasterizeBuffers();

    PostProcessor postProcessor;

    PostProcSettings postProcSettings;

    Rasterizer rasterizer;
    rasterizer.setScene(scene);

    PathTracer pathTracer(displayWidth, displayHeight);
    pathTracer.setScene(scene);

    pathTracer.setTargetp(&renderTarget);
    pathTracer.clearTarget();
    auto tmpSettings = pathTracer.getPathTraceSettings();
    PathTraceSettings initialSettings;
    initialSettings.spp = 1;
    initialSettings.pathSettings.alphaRR = 0.99;
    pathTracer.setPathTraceSettings(initialSettings);

    pathTracer.setRecordingCoords(glm::vec2((displayWidth - 1) / 2 + 35, (displayHeight - 1) / 3));
    pathTracer.enablePathRecording(true);

    pathTracer.startRender();
    RenderState state = pathTracer.getRenderState();
    while (state != RenderState::IDLE) {
        state = pathTracer.doRenderStep();
    }

    PathVisObject pathObj = PathVisObject(pathTracer.getRecordedPath().vertices);
    rasterizer.getSceneRef().addPathObject(pathObj);
    // rasterizer.getSceneRef().getCamera().setLocation(glm::vec3(9, -5, 15));
    // rasterizer.getSceneRef().getCamera().setViewDir(glm::normalize(glm::vec3(-1, 0, -1)));

    // auto pathObj = PathVisObject(vertices);
    // rasterizer.getSceneRef().addPathObject(pathObj);

    pathTracer.setPathTraceSettings(tmpSettings);

    ImGuiMenuSettings menuSettings;

    pathTracer.enablePathRecording(false);

    keyStateManager->getMouseAndGetDiff(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        RenderState state = pathTracer.getRenderState();
        if (state != RenderState::IDLE) {
            pathTracer.doRenderStep();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        buildImguiMenu(menuSettings);

        if (menuSettings.showStatsWindow) {
            buildImguiAppStats(menuSettings);
        }

        if (menuSettings.showControlPanel) {
            buildImguiControlSideBar(glm::vec2(displayWidth, displayHeight), menuSettings, pathTracer, br,
                                     rasterizer, postProcSettings);
        }

        if (state != RenderState::IDLE) pathTracer.buildImGuiProgressBar(glm::vec2(displayWidth, displayHeight));

        ImGui::Render();


        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (menuSettings.viewPathTraceOutput) {
            postProcessor.applyPostProcessing(renderTarget, displayBuffer, postProcSettings);

            br.render(&displayBuffer);
        } else {
            glm::vec2 diff = keyStateManager->getMouseAndGetDiff(window);
            cameraManipulator.updateCamera(rasterizer.getSceneRef().getCamera(), keyStateManager, diff, glm::vec2(displayWidth, displayHeight));

            rasterizer.render();
            rasterizer.renderPaths();
            CameraVisBuffers buffers;
            pathTracer.getSceneRef().getCamera().buildCameraVisBuffers(buffers);
            rasterizer.renderCameraFrustum(buffers);
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void Application::buildImguiControlSideBar(glm::vec2 displaySize, ImGuiMenuSettings &menuSettings, PathTracer &pt,
                                           BufferRenderer &br, Rasterizer &rasterizer, PostProcSettings &postProcSettings) {
    float windowWidth = displaySize.x / 6.0f;


    ImGui::SetNextWindowPos(
            ImVec2(displaySize.x - windowWidth, menuSettings.menuBarSize.y),
            ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, displaySize.y), ImGuiCond_Once);
    ImGui::Begin("Control Panel", &menuSettings.showControlPanel,
                 ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoMove);

    ImGui::Checkbox("Show Pathtrace output", &menuSettings.viewPathTraceOutput);

    ImGui::Indent(8.0f);

    br.buildImGuiWindow();

    ImGui::Unindent(8.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    pt.buildImGuiWindow();

    ImGui::Unindent(8.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    buildImguiCameraControls(pt, rasterizer);

    ImGui::Unindent(8.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    if (ImGui::CollapsingHeader("Post Processing")) {
        ImGui::Checkbox("Gamma Correction", &postProcSettings.gammaCorrect);
        ImGui::Checkbox("tonemapping", &postProcSettings.tonemap);
    }

    ImGui::Unindent(8.0f);

    ImGui::End();
}

void Application::buildImguiCameraControls(PathTracer &pt, Rasterizer &rasterizer) {
    if (ImGui::CollapsingHeader("Camera Controls")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reset debug camera", buttonSize)) {
            rasterizer.getSceneRef().setCamera(pt.getSceneRef().getCamera());
        }

        if (ImGui::Button("Pathtrace camera to debug camera", buttonSize)) {
            pt.getSceneRef().setCamera(rasterizer.getSceneRef().getCamera());
        }

    }
}
