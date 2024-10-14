
#pragma once


#include "CameraManipulator.h"

class OrbitManipulator : public CameraManipulator {

    glm::vec3 mPivot;
    float wheelSpeedFactor;
    float rotateFactorX;
    float rotateFactorY;

public:
    OrbitManipulator()
        : mPivot(0.0f), wheelSpeedFactor(0.1), rotateFactorX(1.0f), rotateFactorY(1.0f) {};

    bool updateCamera(Camera &camera, glm::vec2 extents, glm::vec2 mouseDiff) override;

    std::string getDisplayName() override { return "Orbit Camera"; };

    void buildImgui(Camera &activeCamera) override;
};
