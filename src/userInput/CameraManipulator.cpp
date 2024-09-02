
#include "CameraManipulator.h"
#include "glm/ext/matrix_transform.hpp"

bool CameraManipulator::updateCamera(Camera &camera, KeyStateManager *keyStateManager, glm::vec2 diff, glm::vec2 extents) {
    bool control = keyStateManager->getKeyState(GLFW_KEY_LEFT_CONTROL);
    bool leftMouseDown = keyStateManager->getKeyState(GLFW_MOUSE_BUTTON_LEFT);

    glm::vec3 location = camera.getLocation();
    glm::vec3 viewDir = camera.getViewDir();
    glm::vec3 up = camera.getCamSpaceUpVec();
    glm::vec3 tangent = glm::cross(viewDir, up);

    float posSpeed = 0.05;

    // glm::vec2 diff = keyStateManager->getMouseAndGetDiff(window);
    glm::mat4 rot(1);

    bool updateRot = false;

    if (control && leftMouseDown && diff != glm::vec2(0)) {
        // glm::vec2 extents(displayWidth, displayHeight);
        float fovY = camera.getIntrinsics().fovY;
        glm::vec2 fovs(fovY * camera.getIntrinsics().aspectRatio, fovY);
        glm::vec2 angles = glm::vec2(1) * fovs * (diff / extents);

        rot = glm::rotate(rot, angles.x, up);
        rot = glm::rotate(rot, angles.y, tangent);
        updateRot = true;
    }

    glm::vec3 translation(0);
    if (keyStateManager->getKeyState(GLFW_KEY_W)) translation += viewDir * posSpeed;
    if (keyStateManager->getKeyState(GLFW_KEY_A)) translation -= tangent * posSpeed;
    if (keyStateManager->getKeyState(GLFW_KEY_S)) translation -= viewDir * posSpeed;
    if (keyStateManager->getKeyState(GLFW_KEY_D)) translation += tangent * posSpeed;
    if (keyStateManager->getKeyState(GLFW_KEY_Q)) translation += up * posSpeed;
    if (keyStateManager->getKeyState(GLFW_KEY_E)) translation -= up * posSpeed;

    bool updateTranslate = translation != glm::vec3(0);

    if (updateTranslate) {
        camera.transformPosition(glm::translate(glm::mat4(1), translation));
    }

    if (updateRot) {
        camera.transformViewDir(rot);
    }

    return updateTranslate || updateRot;
}
