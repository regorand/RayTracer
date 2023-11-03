#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "ImguiUtils.h"
#include "renderers/PathTracer.h"
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

    PathTracer pathTracer(displayWidth, displayHeight);
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


