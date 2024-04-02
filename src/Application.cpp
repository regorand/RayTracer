#include <string>
#include "Application.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "renderers/BufferRenderer.h"
#include "gui/ImguiUtils.h"
#include "renderers/PathTracer.h"
#include "scene/OldScene.h"
#include "utils/SceneCreateUtils.h"
#include "renderers/PostProcessor.h"
#include "renderers/Rasterizer.h"
#include "dataStructures/SettingIndex.h"
#include "gui/ImGuiSidebarTab.h"
#include "scene/camera/CameraController.h"
#include "glWrappers/TextureFactory.h"
#include "glWrappers/FrameBufferObject.h"
#include "dataStructures/TextureNames.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/buffers/SceneBuilder.h"
#include "scene/Scene.h"
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

    Scene testScene;
    testScene.loadFile("res/models/cube_green.gltf");

    testScene.buildRasterBuffers();


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

    std::map<int, Texture> textureStore;
    textureStore.insert({RENDER_TARGET, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({DISPLAY_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    textureStore.insert({POS_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({NORMAL_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});
    textureStore.insert({ALBEDO_BUFFER, createPathTraceBuffer(displayWidth, displayHeight)});

    textureStore.insert({DEPTH_BUFFER, createDepthBuffer(displayWidth, displayHeight)});

    OldScene scene;
    Camera camera(glm::vec3(0, 0, -25), glm::vec3(0, 0, 1));
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

    PostProcessor postProcessor(displayWidth, displayHeight);

    Rasterizer rasterizer;
    rasterizer.setScene(scene);

    FrameBufferObject fbo;
    fbo.addTextureAttachment(textureStore.at(POS_BUFFER), GL_COLOR_ATTACHMENT0);
    fbo.addTextureAttachment(textureStore.at(NORMAL_BUFFER), GL_COLOR_ATTACHMENT1);
    fbo.addTextureAttachment(textureStore.at(ALBEDO_BUFFER), GL_COLOR_ATTACHMENT2);
    fbo.addTextureAttachment(textureStore.at(DEPTH_BUFFER), GL_DEPTH_ATTACHMENT);

    PathTracer pathTracer(displayWidth, displayHeight);
    pathTracer.setScene(scene);

    pathTracer.setTargetp(&textureStore.at(RENDER_TARGET));
    pathTracer.clearTarget();

    PathVisObject pathObj = PathVisObject(pathTracer.getRecordedPath().vertices);
    if (pathObj.hasVertices()) {
        rasterizer.getSceneRef().addPathObject(pathObj);
    }

    pathTracer.enablePathRecording(false);

    keyStateManager->getMouseAndGetDiff(window);

    settings.store(SHOW_STATS_WINDOW, false);
    settings.store(SHOW_CONTROL_PANEL, true);

    settings.store(GAMMA_CORRECT, true);

    // CameraController cameraController(pathTracer, rasterizer);

    std::vector<ImGuiSidebarTab *> sideBarTabs;
    sideBarTabs.push_back(&br);
    sideBarTabs.push_back(&pathTracer);
    sideBarTabs.push_back(&scene);
    // sideBarTabs.push_back(&cameraController);
    sideBarTabs.push_back(&postProcessor);


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

        if (settings.getOrElse(SHOW_PATHTRACE_OUTPUT, false)) {
            // postProcessor.applyPostProcessing(renderTarget, displayBuffer, settings);
            postProcessor.applyPostProcessing(textureStore.at(RENDER_TARGET),
                                              textureStore.at(DISPLAY_BUFFER),
                                              textureStore,
                                              settings);

            if (settings.getOrElse(SHOW_PATHTRACE_DEBUG, false)) {
                int currentIndex = settings.getOrElse(PATHTRACE_DEBUG_BUFFER_INDEX, 0);
                int maxIndex = PathTracerBuffers::getAsList().size();
                currentIndex = glm::min(glm::max(currentIndex, 0), maxIndex);

                br.renderStorageBuffer(pathTracer.getBuffers().getByIndex(currentIndex),
                                       glm::vec2(displayWidth, displayHeight));
            } else {
                br.render(&textureStore.at(DISPLAY_BUFFER));
            }
        } else {
            testScene.render(testScene.getActiveCamera());

            glm::vec2 diff = keyStateManager->getMouseAndGetDiff(window);
            cameraManipulator.updateCamera(testScene.getActiveCamera(), keyStateManager, diff,
                                           glm::vec2(displayWidth, displayHeight));

            /*
            glm::vec2 diff = keyStateManager->getMouseAndGetDiff(window);
            cameraManipulator.updateCamera(rasterizer.getSceneRef().getCamera(), keyStateManager, diff,
                                           glm::vec2(displayWidth, displayHeight));

            fbo.bind();


            posBuffer.bind();
            normalBuffer.bind(1);
            albedoBuffer.bind(2);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            rasterizer.saveToBuffers(pathTracer.getSceneRef().getCamera());
            fbo.unbind();

            br.render(&textureStore.at(NORMAL_BUFFER));

            // rasterizer.render();
            // rasterizer.renderPaths();
            // rasterizer.renderCameraFrustum(buffers);
            CameraVisBuffers buffers;
            pathTracer.getSceneRef().getCamera().buildCameraVisBuffers(buffers);

            */
        }

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);






        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    for (auto &texture : textureStore) {
        texture.second.destroy();
    }

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
            rasterizer.getSceneRef().setCamera(pt.getSceneRef().getCamera());
        }

        if (ImGui::Button("Pathtrace camera to debug camera", buttonSize)) {
            pt.getSceneRef().setCamera(rasterizer.getSceneRef().getCamera());
        }

    }
}

void Application::clearTexture(Texture &texture, ComputeShader shader) {
    texture.bindAsImage(0);
    shader.bind();

    glDispatchCompute(texture.getWidth(), texture.getHeight(), 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}
