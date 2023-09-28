#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "temp/PerlinNoise.hpp"
#include "RayTracer.h"
#include "glm/gtc/type_ptr.hpp"
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

    /*
    for (int x = 0; x < displayWidth; x++) {
        for (int y = 0; y < displayHeight; y++) {
            float factor = 50;

            float x_fac = factor * static_cast<float>(x) / displayWidth;
            float y_fac = factor * static_cast<float>(y) / displayHeight;

            float noiseR = perlinR.normalizedOctave2D_01(x_fac, y_fac, 4, 0.9) ;
            float noiseG = perlinG.normalizedOctave2D_01(x_fac, y_fac, 4, 0.9) ;

            if (noiseG < 0.75) noiseG = 0;

            float noiseB = perlinB.normalizedOctave2D_01(x_fac, y_fac, 4, 0.9) ;

            // glm::vec3 color = glm::vec3(noiseR);

            float facR = 235.0 / 255.0;
            float facG = 187.0 / 255.0;
            float facB = 171.0 / 255.0;

            glm::vec3 color = glm::vec3(noiseR * facR, noiseR * facG, noiseR * facB);
            float threshold = 0.3;
            float upperThreshold = 0.68;
            if (noiseR > threshold) {
                float t = 0;
                if (noiseR > upperThreshold) {
                    t = 0;
                } else {
                    t = (1 - upperThreshold) / (1 - upperThreshold);
                }


                color = t * color + (1 - t) * glm::vec3(noiseR);
            }


            buffer.setPixel(x, y, color);

            /*
            float val1 = static_cast<float>(std::rand()) / RAND_MAX;
            float val2 = static_cast<float>(std::rand()) / RAND_MAX;
            float val3 = static_cast<float>(std::rand()) / RAND_MAX;

            buffer.setPixel(x, y, glm::vec3(val1, val2, val3));

        }

    };
      */

    RayTracer rayTracer;
    Scene scene;
    rayTracer.setScene(&scene);
    rayTracer.setHostBuffer(&buffer);
    rayTracer.render();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Hello, world!");

            if (ImGui::Button("Reload shaders"))
                br.reloadShaders();

            if (ImGui::Button("Render"))
                rayTracer.render();

            ImGui::SliderFloat3("Camera pos", glm::value_ptr(scene.getCamera().getLocation()), -100.0, 100.0);
            glm::vec3 dir = scene.getCamera().getViewDir();
            ImGui::SliderFloat3("Camera dir", glm::value_ptr(dir), 0, 1);
            scene.getCamera().setViewDir(glm::normalize(dir));

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();

        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT);



        br.render(&buffer);

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
