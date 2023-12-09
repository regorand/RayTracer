
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include "glm/vec3.hpp"
#include "scene/Camera.h"
#include "renderers/Types.h"
#include "glWrappers/shaders/SSBO.h"
#include "MeshObject.h"
#include "SceneBuffers.h"
#include "PathVisObject.h"

class Scene {

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
};


#endif //RAYTRACER_SCENE_H
