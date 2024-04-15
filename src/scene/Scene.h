#pragma once


#include "scene/camera/CameraController.h"
#include "tiny_gltf.h"
#include "SceneGraph.h"
#include "Material.h"
#include "Mesh.h"
#include "Vertex.h"
#include "scene/buffers/RasterizeBuffers.h"
#include "glWrappers/GLBuffer.h"
#include "glWrappers/shaders/ShaderProgram.h"

class Scene : public ImGuiSidebarTab {
    CameraController cameraController;

    int activeCameraIndex = DEFAULT_CAMERA_INDEX;

    tinygltf::Model model;
    GraphNode sceneGraphRoot;

    std::vector<Mesh> mMeshes;
    std::vector<Material> mMaterials;
    SSBO materialBuffer;

    RasterBuffers mRasterBuffers;
    ShaderProgram shaderProgram;

public:
    bool loadFile(std::string path);
    ~Scene();

    void addCamera(Camera camera);
    Camera &getActiveCamera();

    void reloadShaders();

    void buildBuffers();

    void render(Camera &camera);
    void renderNode(GraphNode &node, glm::mat4 transform, glm::mat4 &VP);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;
private:

    void buildRasterBuffersNode(GraphNode node, std::vector<Vertex> &data, std::vector<unsigned int> &indices);

    void buildMaterialBuffer();
};
