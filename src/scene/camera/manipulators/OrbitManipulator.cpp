
#include <algorithm>
#include "OrbitManipulator.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

bool OrbitManipulator::updateCamera(Camera &camera, glm::vec2 extents, glm::vec2 mouseDiff) {
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) return false;
    constexpr float EPS_4 = 1e-4f;
    bool leftMouseDown = ImGui::IsKeyDown(ImGuiKey_MouseLeft);
    bool middleMouseDown = ImGui::IsKeyDown(ImGuiKey_MouseMiddle);


    glm::vec3 pos = camera.getLocation();
    float dist = glm::distance(pos, mPivot);

    glm::vec3 dir = glm::normalize(pos - mPivot);

    glm::vec3 U = camera.getLocalUpVec();
    glm::vec3 T = glm::cross(U, dir);

    bool movedPivot = false;
    if (middleMouseDown) {
        glm::vec2 movement = glm::vec2(dist) * mouseDiff / extents;

        if (std::fabs(movement.x) >= EPS_4 || std::fabs(movement.y) >= EPS_4) {
            movedPivot = true;
            glm::vec3 worldDiff = -movement.x * T + movement.y * U ;
            mPivot += worldDiff;
            pos += worldDiff;
        }
    }

    glm::vec3 localPos = pos - mPivot;

    float wheel = io.MouseWheel;
    bool changedDist = std::fabs(wheel) >= EPS_4;
    dist *= 1 - (wheelSpeedFactor * wheel);


    glm::vec2 correctedMouseDiffs = glm::vec2(glm::two_pi<float>(), glm::pi<float>()) * mouseDiff / extents;

    float theta = rotateFactorX * -std::atan(correctedMouseDiffs.x);
    float phi = rotateFactorY * -std::atan(correctedMouseDiffs.y);

    glm::vec3 rotated = localPos;
    bool changedRot = false;
    if (leftMouseDown) {
        float currentPhi = glm::half_pi<float>() - std::acos(glm::dot(U, UP_ALIGNMENT));
        if (phi + currentPhi <= 0) phi = -currentPhi + 1e-3f;
        if (phi + currentPhi >= glm::pi<float>()) phi = glm::pi<float>() - currentPhi - 1e-3f;

        glm::mat4 rot(1);
        rot = glm::rotate(rot, theta, U);
        rot = glm::rotate(rot, phi, T);

        changedRot = std::fabs(theta) > EPS_4 || std::fabs(phi) > EPS_4;
        rotated = rot * glm::vec4(localPos, 0);
    }
    glm::vec3 newLocalPos = dist * glm::normalize(rotated);

    bool changed = changedDist || changedRot || movedPivot;

    if (changed) {

        glm::vec3 newPos = newLocalPos + mPivot;
        glm::vec3 newDir = glm::normalize(mPivot - newPos);

        camera.setLocation(newPos);
        camera.setViewDir(newDir);
    }

    return changed;
}

void OrbitManipulator::buildImgui(Camera &activeCamera) {
    ImGui::InputFloat3("Pivot", glm::value_ptr(mPivot));
    if (ImGui::Button("Pivot to World origin")) {
        activeCamera.setLocation(activeCamera.getLocation() - mPivot);
        mPivot = glm::vec3(0);
    }
}
