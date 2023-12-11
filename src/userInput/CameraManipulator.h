#pragma once

#include "utils/glewIncludeUtils.h"
#include "KeyStateManager.h"
#include "scene/Camera.h"

class CameraManipulator {


public:

    void updateCamera(Camera &camera, KeyStateManager *keyStateManager, glm::vec2 diff, glm::vec2 extents);

};