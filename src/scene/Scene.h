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

    int activeCameraIndex = DEFAULT_CAMERA_INDEX;

    SceneBuffers sceneBuffers;

    /* TODO new struct combining this**/
    ShaderStorageBuffer triangleBuffer;
    ShaderStorageBuffer objectBuffer;
    ShaderStorageBuffer materialBuffer;
    ShaderStorageBuffer transformationBuffer;
    ShaderStorageBuffer emissiveBuffer;


    RasterBuffers mRasterBuffersNew; // These are the new ones, the ones that should stay eventually
    ShaderProgram shaderProgram;

public:
    bool loadFile(std::string path);
    ~Scene();

    void addCamera(Camera camera);
    Camera &getActiveCamera();

    void reloadShaders();

    void render(Camera &camera);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
private:
    void buildMaterialBuffer();
};
