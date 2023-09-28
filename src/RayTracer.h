
#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

#include "glm/vec3.hpp"
#include "HostBuffer.h"
#include "Scene.h"



class RayTracer {
private:
    HostBuffer *mHostBuffer = nullptr;
    Scene *mScene = nullptr;

public:

    void setHostBuffer(HostBuffer *hostBuffer);
    void setScene(Scene *scene);

    void render();

private:
    glm::vec3 raytrace();

};


#endif //RAYTRACER_RAYTRACER_H
