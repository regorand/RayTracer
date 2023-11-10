
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include "glm/vec3.hpp"
#include "scene/Camera.h"
#include "renderers/Types.h"
#include "glWrappers/shaders/SSBO.h"
#include "MeshObject.h"
#include "SceneBuffers.h"

class Scene {

private:
    std::vector<MeshObject> mObjects;

    SceneBuffers sceneBuffers;

    Camera mCamera;

    unsigned int mObjectCount;
    unsigned int mEmissiveCount;

public:
    Scene();

    void setCamera(Camera camera);

    void addMeshObject(MeshObject &meshObject);

    void createGPUBuffers();

    bool hasGPUBuffers() { return sceneBuffers.isValid(); };

    unsigned int getObjectCount() { return mObjectCount; };
    unsigned int getEmissiveCount() {return mEmissiveCount; };

    void bind();
    void unbind();

};


#endif //RAYTRACER_SCENE_H
