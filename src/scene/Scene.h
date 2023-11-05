
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include "glm/vec3.hpp"
#include "scene/Camera.h"
#include "deviceShared/Types.h"
#include "glWrappers/shaders/SSBO.h"
#include "MeshObject.h"

class Scene {

private:
    // std::vector<MeshObject> mObjects;

    std::vector<Triangle> mTriangles;
    Camera mCamera;

    SSBO mSSBO;

public:
    Scene();

    void setCamera(Camera camera);


    void addMeshObject(MeshObject meshObject);
    void addTriangle(Triangle triangle);

    void createSSBO();

    bool hasValidSSBO() { return mSSBO.hasData(); };

    SSBO &getSSBO();


};


#endif //RAYTRACER_SCENE_H
