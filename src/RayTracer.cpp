#include <stdexcept>
#include "glm/trigonometric.hpp"
#include "RayTracer.h"
#include "geometry/Intersections.hpp"

void RayTracer::setHostBuffer(HostBuffer *hostBuffer) {
    mHostBuffer = hostBuffer;
}

void RayTracer::setScene(Scene *scene) {
    mScene = scene;
}

void RayTracer::render() {
    if (mHostBuffer == nullptr || mScene == nullptr) std::runtime_error("Renderer not fully initilized");

    Camera camera = mScene->getCamera();
    Intrinsics intrinsics = camera.getIntrinsics();

    int width = mHostBuffer->getWidth();
    int height = mHostBuffer->getHeight();

    float aspect = static_cast<float>(height) / width;

    float sideDist = glm::tan(intrinsics.fov / 2);
    float upDist = sideDist * aspect;

    float halfX = static_cast<float>(width) / 2;
    float halfY = static_cast<float>(height) / 2;

    glm::vec3 camDir = camera.getViewDir();
    glm::vec3 up = camera.getUpVec();
    glm::vec3 tangentVec = glm::cross(camDir, up);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float shiftedScaledX = static_cast<float>(x - halfX) / halfX;
            float shiftedScaledY = static_cast<float>(y - halfY) / halfY;

            glm::vec3 rayDir = camDir + shiftedScaledX * sideDist * tangentVec + shiftedScaledY * upDist * up;
            rayDir = glm::normalize(rayDir);

            Ray ray;
            ray.origin = camDir;
            ray.direction = rayDir;

            mHostBuffer->setPixel(x, y, glm::vec3(0));

            bool hitAnything = false;
            float closestHit = INFINITY;
            Sphere hitSphere;
            for (auto &sphere: mScene->getSpheres()) {
                float dist = 0;
                bool hit = intersectSphere(ray, sphere, &dist);
                if (hit && dist > 0) {
                    hitAnything = true;
                    if (dist < closestHit) {
                        closestHit = dist;
                        hitSphere = sphere;
                    }
                }
            }
            if (hitAnything) {
                glm::vec3 pos = ray.origin + closestHit * ray.direction;
                glm::vec3 normal = glm::normalize(pos - hitSphere.center);

                float factor = glm::clamp(glm::dot(normal, -ray.direction), 0.0f, 1.0f);
                glm::vec3 color = factor * hitSphere.material.color;
                mHostBuffer->setPixel(x, y, color);
            }

            // mHostBuffer->setPixel(x, y, rayDir);
        }
    }
}
