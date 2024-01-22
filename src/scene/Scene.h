
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include "glm/vec3.hpp"
#include "scene/camera/Camera.h"
#include "renderers/Types.h"
#include "glWrappers/shaders/SSBO.h"
#include "MeshObject.h"
#include "SceneBuffers.h"
#include "PathVisObject.h"
#include "dataStructures/DataStore.h"
#include "dataStructures/Events.h"
#include "gui/ImGuiSidebarTab.h"

class Scene : public ImGuiSidebarTab {

private:
    std::vector<MeshObject> mObjects;
    std::vector<PathVisObject> mPaths;

    SceneBuffers sceneBuffers;

    Camera mCamera;

    unsigned int mObjectCount;
    unsigned int mEmissiveCount;

public:
    Scene();

    void setCamera(Camera camera);
    Camera &getCamera();

    void addMeshObject(MeshObject &meshObject);
    void addPathObject(PathVisObject &pathVisObject);

    std::vector<MeshObject> &getObjects();
    std::vector<PathVisObject> &getPathObjects();

    void createGPUBuffers();

    bool hasGPUBuffers() { return sceneBuffers.isValid(); };

    unsigned int getObjectCount() { return mObjectCount; };
    unsigned int getEmissiveCount() {return mEmissiveCount; };

    void bind();
    void unbind();

    void buildRasterizeBuffers();
    bool canRasterize();

    void resetPaths();

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

private:
    void buildImguiObjectList();
    void buildObjectImgui(MeshObject &object, size_t index);
};


#endif //RAYTRACER_SCENE_H
