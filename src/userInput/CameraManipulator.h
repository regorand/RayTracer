#pragma once

#include "utils/glewIncludeUtils.h"
#include "KeyStateManager.h"
#include "scene/camera/Camera.h"

class CameraManipulator {


public:

    bool updateCamera(Camera &camera, glm::vec2 extents);

};