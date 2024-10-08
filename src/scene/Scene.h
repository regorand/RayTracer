#pragma once


#include "scene/camera/CameraController.h"
#include "tiny_gltf.h"
#include "Material.h"
#include "scene/buffers/RasterizeBuffers.h"
#include "glWrappers/GLBuffer.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "SceneBuffers.h"

class Scene : public ImGuiSidebarTab {
    CameraController cameraController;

    std::string mPath;

    int activeCameraIndex = DEFAULT_CAMERA_INDEX;

    CPUSceneBuffers cpuBuffers;
    GPUSceneBuffers gpuBuffers;

    RasterBuffers mRasterBuffersNew;
public:
    bool loadFile(std::string path);
    ~Scene();

    void addCamera(Camera camera);
    Camera &getActiveCamera();

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
