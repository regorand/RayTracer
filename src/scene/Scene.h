#pragma once


#include "scene/camera/CameraController.h"
#include "tiny_gltf.h"
#include "SceneGraph.h"
#include "Material.h"
#include "Mesh.h"
#include "Vertex.h"
#include "scene/buffers/RasterizeBuffers.h"

class Scene {
    CameraController cameraController;

    int activeCameraIndex = DEFAULT_CAMERA_INDEX;

    tinygltf::Model model;
    GraphNode sceneGraphRoot;

    std::vector<Mesh> mMeshes;
    std::vector<Material> mMaterials;

    RasterBuffers mRasterBuffers;

public:
    bool loadFile(std::string path);

    void addCamera(Camera camera);
    Camera &getActiveCamera();


    void buildRasterBuffers();
    void buildPathtraceBuffers();

    void render(Camera &camera);

    void renderNode(GraphNode &node, Shader &shader, glm::mat4 transform, glm::mat4 &VP);

private:

    void buildRasterBuffersNode(GraphNode node, std::vector<Vertex> &data, std::vector<unsigned int> &indices);

};
