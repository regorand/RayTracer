
#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H


#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

typedef struct {
    float fov;
    float aspectRatio;
} Intrinsics;

class Camera {
private:
    glm::vec3 mPosition;
    glm::vec3 mViewDir;
    const glm::vec3 UP_VECTOR = glm::vec3(0, 1, 0);

    Intrinsics mIntrinsics;

public:
    Camera(glm::vec3 position, glm::vec3 viewDir);

    glm::mat4 getTransformation();


    [[nodiscard]] glm::vec3 &getLocation();
    [[nodiscard]] glm::vec3 getUpVec();
    [[nodiscard]] glm::vec3 &getViewDir();

    void setLocation(glm::vec3 location);
    void setViewDir(glm::vec3 viewDir);

    [[nodiscard]] Intrinsics getIntrinsics() const {return mIntrinsics; };
};


#endif //RAYTRACER_CAMERA_H
