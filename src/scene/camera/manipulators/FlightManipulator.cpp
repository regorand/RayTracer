
#include "FlightManipulator.h"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

bool FlightManipulator::updateCamera(Camera &camera, glm::vec2 extents, glm::vec2 mouseDiff) {
    bool control = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);
    bool leftMouseDown = ImGui::IsKeyDown(ImGuiKey_MouseLeft);

    glm::vec3 location = camera.getLocation();
    glm::vec3 viewDir = camera.getViewDir();
    glm::vec3 up = camera.getCamSpaceUpVec();
    glm::vec3 tangent = glm::cross(viewDir, up);

    ImGuiIO &io = ImGui::GetIO();
    // glm::vec2 diff = glm::vec2(io.MouseDelta.x, io.MouseDelta.y);

    float posSpeed = 0.05;

    glm::mat4 rot(1);

    bool updateRot = false;

    if (control && leftMouseDown && mouseDiff != glm::vec2(0)) {
        float fovY = camera.getIntrinsics().fovY;
        glm::vec2 fovs(fovY * camera.getIntrinsics().aspectRatio, fovY);
        glm::vec2 angles = glm::vec2(1) * fovs * (mouseDiff / extents);

        rot = glm::rotate(rot, angles.x, up);
        rot = glm::rotate(rot, angles.y, tangent);
        updateRot = true;
    }

    glm::vec3 translation(0);

    if (ImGui::IsKeyDown(ImGuiKey_W)) translation += viewDir * posSpeed;
    if (ImGui::IsKeyDown(ImGuiKey_A)) translation -= tangent * posSpeed;
    if (ImGui::IsKeyDown(ImGuiKey_S)) translation -= viewDir * posSpeed;
    if (ImGui::IsKeyDown(ImGuiKey_D)) translation += tangent * posSpeed;
    if (ImGui::IsKeyDown(ImGuiKey_Q)) translation += up * posSpeed;
    if (ImGui::IsKeyDown(ImGuiKey_E)) translation -= up * posSpeed;

    bool updateTranslate = translation != glm::vec3(0);

    if (updateTranslate) {
        camera.transformPosition(glm::translate(glm::mat4(1), translation));
    }

    if (updateRot) {
        camera.transformViewDir(rot);
    }

    return updateTranslate || updateRot;
}

void FlightManipulator::buildImgui(Camera &activeCamera) {

}
