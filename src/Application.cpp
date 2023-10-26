#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "temp/PerlinNoise.hpp"
#include "RayTracer.h"
#include "glm/gtc/type_ptr.hpp"
#include "glWrappers/shaders/ComputeShader.h"
#include "renderers/ComputeRender.h"
#include "glWrappers/shaders/ShaderLoad.h"
#include <imgui.h>

Application::Application() { }





int Application::run() {

    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    const char* glsl_version = "#version 130";
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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
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


    const siv::PerlinNoise::seed_type seedR = 123456u;
    const siv::PerlinNoise::seed_type seedG = 654321u;
    const siv::PerlinNoise::seed_type seedB = 246810u;

    const siv::PerlinNoise perlinR{seedR };
    const siv::PerlinNoise perlinG{seedG };
    const siv::PerlinNoise perlinB{seedB };


    RayTracer rayTracer;
    Scene scene;
    Texture texture(&buffer);

    /*
    rayTracer.setScene(&scene);
    rayTracer.setHostBuffer(&buffer);
    rayTracer.render();
    texture.updateTexture(buffer.getData());
    */

    ComputeShader computeShader("res/shaders/compute/computeRayTracer.comp");
    // ComputeShader computeShader("res/shaders/includeTest/includeMain.comp");
    // ComputeShader computeShader("res/shaders/compute/computeNoBounces.comp");

    computeSimple(&texture, &computeShader);

    bool alwaysRenderNew = false;
    bool showDebugWindow = false;
    bool resolve = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Control Panel");

            if (ImGui::Button("Reload display shaders"))
                br.reloadShaders();
            ImGui::SameLine();
            if (ImGui::Button("Reload compute shaders"))
                computeShader.reloadShader();

            ImGui::Checkbox("Always render new", &alwaysRenderNew);
            if (alwaysRenderNew || ImGui::Button("Render")) {
                computeSimple(&texture, &computeShader);
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            std::string debugButtonString = std::string(showDebugWindow ? "Hide " : "Show ") + "debug window";
            if (ImGui::Button(debugButtonString.c_str())) {
                showDebugWindow = !showDebugWindow;
            }
            ImGui::End();

            if (showDebugWindow) {
                ImGui::Begin("Debug");
                ImGui::Checkbox("Resolve Custom Symbols", &resolve);

                if (ImGui::Button("Test Shader Include Module")) {
                    std::string testSrc = loadShaderSource("res/shaders/includeTest/includeMain.comp", resolve);
                    std::cout << testSrc << std::endl;
                }

                ImGui::End();
            }

        }

        ImGui::Render();

        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT);


        br.render(&texture);

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
