
#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H

#include <vector>
#include "glm/vec3.hpp"
#include "scene/Camera.h"
#include "geometry/Objects.h"


class Scene {

private:
    std::vector<Sphere> mSpheres;
    Camera mCamera;

public:
    Scene();

    std::vector<Sphere> &getSpheres();
    Camera &getCamera();
};


#endif //RAYTRACER_SCENE_H
