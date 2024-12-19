#pragma once

#include "tiny_gltf.h"
#include "Material.h"
#include "scene/buffers/RasterizeBuffers.h"
#include "glWrappers/GLBuffer.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "SceneBuffers.h"
#include "gui/ImGuiSidebarTab.h"
#include "scene/camera/Camera.h"

class Scene : public ImGuiSidebarTab {
    std::string mPath;

    std::vector<Camera> staticCameras;
    Camera freeCamera;

    int hoveredCameraIndex = -1;

    CPUSceneBuffers cpuBuffers;
    GPUSceneBuffers gpuBuffers;

    RasterBuffers mRasterBuffersNew;
public:
    Scene() = default;

    bool loadFile(std::string path);
    ~Scene();

    Camera &getActiveCamera();
    std::vector<Camera> &getStaticCameras();
    int getHoveredCameraIndex();

    std::vector<Object> &getObjects();
    std::vector<Mesh> &getMeshes();

    CPUSceneBuffers &getCPUBuffers();
    GPUSceneBuffers &getGPUBuffers();

    ShaderStorageBuffer &getMatBuffer();

    std::string getPath();
    std::string getName();

    void bindRaster();
    void unbindRaster();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
private:
    void buildMaterialBuffer();
    void buildMeshBuffer();

    void buildRasterBuffers();

    void buildGPUBuffers();
};
