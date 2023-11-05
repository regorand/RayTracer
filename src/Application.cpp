#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "ImguiUtils.h"
#include "renderers/PathTracer.h"
#include "glWrappers/shaders/ShaderLoad.h"
#include "scene/Scene.h"
#include <imgui.h>

Application::Application() {}


int Application::run() {

    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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
    registerGLCallBack();

    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

    BufferRenderer br;

    HostBuffer buffer(displayWidth, displayHeight);

    Texture texture(&buffer);
    texture.setAccessType(GL_READ_WRITE);

    size_t VS = sizeof(Vertex);
    size_t TS = sizeof(Triangle);

    Scene scene;
    Camera camera(glm::vec3(0, 0, 30), glm::vec3(0, 0, -1));
    scene.setCamera(camera);
    Vertex V0_0;
    Vertex V1_0;
    Vertex V2_0;

    V0_0.pos = glm::vec3(0, 0, 0);
    V1_0.pos = glm::vec3(0, 0, 0);
    V2_0.pos = glm::vec3(0, 0, 0);

    V0_0.normal = glm::vec3(0, 0, 0);
    V1_0.normal = glm::vec3(0, 0, 0);
    V2_0.normal = glm::vec3(0, 0, 0);

    V0_0.uv = glm::vec2(1, 1);
    V1_0.uv = glm::vec2(1, 0);
    V2_0.uv = glm::vec2(0, 1);



    Triangle tri_0;
    tri_0.V0 = V0_0;
    tri_0.V1 = V1_0;
    tri_0.V2 = V2_0;
    tri_0.color = glm::vec3(0, 0, 0);

    Vertex V0;
    Vertex V1;
    Vertex V2;

    V0.pos = glm::vec3(10, 10, -10);
    V1.pos = glm::vec3(10, -10, 0);
    V2.pos = glm::vec3(-10, 10, 0);

    V0.normal = glm::vec3(0, 0, 1);
    V1.normal = glm::vec3(1, 0, 1);
    V2.normal = glm::vec3(0, 1, 1);

    V0.uv = glm::vec2(1, 1);
    V1.uv = glm::vec2(1, 0);
    V2.uv = glm::vec2(0, 1);

    Triangle tri;
    tri.V0 = V0;
    tri.V1 = V1;
    tri.V2 = V2;
    tri.color = glm::vec3(1, 0, 0);

    scene.addTriangle(tri);
    // scene.addTriangle(tri_0);
    scene.createSSBO();

    PathTracer pathTracer(displayWidth, displayHeight);
    pathTracer.setScene(scene);

    pathTracer.setTargetp(&texture);
    pathTracer.clearTarget();
    auto tmpSettings = pathTracer.getPathTraceSettings();
    PathTraceSettings initialSettings;
    initialSettings.spp = 1;
    pathTracer.setPathTraceSettings(initialSettings);
    pathTracer.startRender();
    RenderState state = pathTracer.getRenderState();
    while (state != RenderState::IDLE) {
        state = pathTracer.doRenderStep();
    }
    pathTracer.setPathTraceSettings(tmpSettings);

    ImGuiMenuSettings menuSettings;

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
            buildImguiControlSideBar(glm::vec2(displayWidth, displayHeight), menuSettings, pathTracer, br);
        }

        if (state != RenderState::IDLE) pathTracer.buildImGuiProgressBar(glm::vec2(displayWidth, displayHeight));

        ImGui::Render();

        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT);
        br.render(&texture);

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

void Application::buildImguiControlSideBar(glm::vec2 displaySize, ImGuiMenuSettings menuSettings, PathTracer &pt,
                                           BufferRenderer &br) {
    float windowWidth = displaySize.x / 6.0f;

    ImGui::SetNextWindowPos(
            ImVec2(displaySize.x - windowWidth, menuSettings.menuBarSize.y),
            ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, displaySize.y), ImGuiCond_Once);
    ImGui::Begin("Control Panel", &menuSettings.showControlPanel,
                 ImGuiWindowFlags_NoResize
                 | ImGuiWindowFlags_NoMove);

    ImGui::Indent(8.0f);

    br.buildImGuiWindow();

    ImGui::Unindent(8.0f);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Indent(8.0f);

    pt.buildImGuiWindow();

    ImGui::Unindent(8.0f);

    ImGui::End();
}


