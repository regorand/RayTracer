
#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H


#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glWrappers/ArrayBuffer.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/IndexBuffer.h"

typedef struct {
    ArrayBuffer arrayBuffer;
    VertexArray vertexArray;
    IndexBuffer indexBuffer;
} CameraVisBuffers ;

typedef struct {
    float fovY;
    float aspectRatio;
} Intrinsics;

static const glm::vec3 UP_ALIGNMENT = glm::vec3(0, 1, 0);

class Camera {
private:
    glm::vec3 mPosition;
    glm::vec3 mViewDir;

    glm::vec3 mCameraSpaceUp;
    glm::vec3 mLocalUp;

    Intrinsics mIntrinsics;

public:
    Camera(glm::vec3 position, glm::vec3 viewDir, glm::vec3 up = glm::vec3(0, 1, 0));

    glm::mat4 getTransformation();
    glm::mat4 getProjection();

    void applyTransform(glm::mat4 transform);

    void transformPosition(glm::mat4 transform);
    void transformViewDir(glm::mat4 transform);
    void transformCamSpaceUpDir(glm::mat4 transform);

    [[nodiscard]] glm::vec3 &getLocation();
    [[nodiscard]] glm::vec3 getCamSpaceUpVec();
    [[nodiscard]] glm::vec3 getLocalUpVec();
    [[nodiscard]] glm::vec3 &getViewDir();

    void setLocation(glm::vec3 location);
    void setViewDir(glm::vec3 viewDir);

    void setAspect(float aspect);

    [[nodiscard]] Intrinsics getIntrinsics() {return mIntrinsics; };

    void buildCameraVisBuffers(CameraVisBuffers &buffers);

private:
    void calculateLocalUp();

};


#endif //RAYTRACER_CAMERA_H
