#pragma once

#include "utils/glewIncludeUtils.h"
#include "userInput/KeyStateManager.h"
#include "scene/camera/Camera.h"
#include "CameraManipulator.h"

class FlightManipulator : public CameraManipulator {

public:
    bool updateCamera(Camera &camera, glm::vec2 extents, glm::vec2 mouseDiff) override;

    std::string getDisplayName() override { return "Fly Camera"; };

    void buildImgui(Camera &activeCamera) override;
};