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

Application::Application()
{
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

    glfwSetKeyCallback(window, glfwKeyCallback);

    registerGLCallBack();

    int displayWidth, displayHeight;
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

    pathTracer.setRecordingCoords(glm::vec2((displayWidth - 1) / 3, (displayHeight - 1) / 3));
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

        handleInput(rasterizer.getSceneRef().getCamera());

        if (menuSettings.showStatsWindow) {
            buildImguiAppStats(menuSettings);
        }

        if (menuSettings.showControlPanel) {
            buildImguiControlSideBar(glm::vec2(displayWidth, displayHeight), menuSettings, pathTracer, br, postProcSettings);
        }

        if (state != RenderState::IDLE) pathTracer.buildImGuiProgressBar(glm::vec2(displayWidth, displayHeight));

        ImGui::Render();


        glViewport(0, 0, displayWidth, displayHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (menuSettings.viewPathTraceOutput) {
            postProcessor.applyPostProcessing(renderTarget, displayBuffer, postProcSettings);

            br.render(&displayBuffer);
        } else {
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
                                           BufferRenderer &br, PostProcSettings &postProcSettings) {
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

    if (ImGui::CollapsingHeader("Post Processing")) {
        ImGui::Checkbox("Gamma Correction", &postProcSettings.gammaCorrect);
        ImGui::Checkbox("tonemapping", &postProcSettings.tonemap);
    }

    ImGui::Unindent(8.0f);

    ImGui::End();
}

void Application::handleInput(Camera &camera) {
    bool space = movementKeys[6];

    glm::vec3 location = camera.getLocation();
    glm::vec3 viewDir = camera.getViewDir();
    glm::vec3 up = camera.getUpVec();
    glm::vec3 tangent = glm::cross(viewDir, up);

    float posSpeed = 0.05;
    float viewDirSpeed = 0.01;

    // w
    if (movementKeys[0]) {
        if (space) {
            viewDir.y += viewDirSpeed;
        } else {
            location += viewDir * posSpeed;
        }
    }

    // a
    if (movementKeys[1]) {
        if (space) {
            viewDir.x -= viewDirSpeed;
        } else {
            location -= tangent * posSpeed;
        }
    }

    // s
    if (movementKeys[2]) {
        if (space) {
            viewDir.y -= viewDirSpeed;
        } else {
            location -= viewDir * posSpeed;
        }
    }

    // d
    if (movementKeys[3]) {
        if (space) {
            viewDir.x += viewDirSpeed;
        } else {
            location += tangent * posSpeed;
        }
    }

    // q
    if (movementKeys[4]) {
        if (!space) {
            location += up * posSpeed;
        }
    }

    // e
    if (movementKeys[5]) {
        if (!space) {
            location -= up * posSpeed;
        }
    }

    camera.setLocation(location);
    camera.setViewDir(glm::normalize(viewDir));
}

void Application::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W) {
        if (action == GLFW_RELEASE) {
            movementKeys[0] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[0] = true;
        }
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_RELEASE) {
            movementKeys[1] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[1] = true;
        }    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_RELEASE) {
            movementKeys[2] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[2] = true;
        }    }

    if (key == GLFW_KEY_D) {
        if (action == GLFW_RELEASE) {
            movementKeys[3] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[3] = true;
        }    }

    if (key == GLFW_KEY_Q) {
        if (action == GLFW_RELEASE) {
            movementKeys[4] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[4] = true;
        }    }

    if (key == GLFW_KEY_E) {
        if (action == GLFW_RELEASE) {
            movementKeys[5] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[5] = true;
        }    }

    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_RELEASE) {
            movementKeys[6] = false;
        } else if (action == GLFW_PRESS) {
            movementKeys[6] = true;
        }    }
}


