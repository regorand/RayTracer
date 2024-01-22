#pragma once

#include "utils/glewIncludeUtils.h"
#include "KeyStateManager.h"
#include "scene/camera/Camera.h"

class CameraManipulator {


public:

    void updateCamera(Camera &camera, KeyStateManager *keyStateManager, glm::vec2 diff, glm::vec2 extents);

};