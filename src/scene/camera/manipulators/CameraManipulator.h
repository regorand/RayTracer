#pragma once
#include "scene/camera/Camera.h"
#include "imgui.h"

class CameraManipulator {
public:
    bool updateCamera(Camera &camera, glm::vec2 extents) {
        ImGuiIO &io = ImGui::GetIO();
        glm::vec2 diff = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

        return updateCamera(camera, extents, diff);
    }

    virtual bool updateCamera(Camera &camera, glm::vec2 extents, glm::vec2 mouseDiff) = 0;

    virtual std::string getDisplayName() = 0;

    virtual void buildImgui(Camera &activeCamera) = 0;
};
