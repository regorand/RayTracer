#pragma once


#include "scene/camera/CameraController.h"
#include "tiny_gltf.h"
#include "SceneGraph.h"
#include "Material.h"
#include "Vertex.h"
#include "scene/buffers/RasterizeBuffers.h"
#include "glWrappers/GLBuffer.h"
#include "glWrappers/shaders/ShaderProgram.h"
#include "SceneGraphTraversal.h"
#include "SceneBuffers.h"

typedef struct iterate_buffers_s {
    std::vector<Vertex_Old> data;
    std::vector<unsigned int> indices;
} IterateBuffers;

inline void bufferBuildPrimitiveCallback(const TraversalIndices &traversalIndices, const tinygltf::Primitive &primitive, void *userData) {
    std::cout   << "Parsing primitive:\nScene index: " << traversalIndices.currentScene
                << "\nMesh: index: " << traversalIndices.currentMesh
                << "\nPrimitive index: " << traversalIndices.currentPrimitive
                << "\n" << std::endl;
    if (userData != nullptr) {
        IterateBuffers &buffers = *(IterateBuffers *) userData;
        std::cout << buffers.data.size();
    }
}

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
