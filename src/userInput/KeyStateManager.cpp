#include "KeyStateManager.h"

KeyStateManager *KeyStateManager::_instance = nullptr;;

KeyStateManager *KeyStateManager::getInstance() {
    if (_instance == nullptr) {
        _instance = new KeyStateManager();
    }
    return _instance;
}

KeyStateManager::KeyStateManager() {
}

glm::vec2 KeyStateManager::getMouseAndGetDiff(GLFWwindow *window) {
    double newX, newY;

    glfwGetCursorPos(window, &newX, &newY);

    glm::vec2 newPos(static_cast<float>(newX), static_cast<float>(newY));
    auto diff = newPos - prevMousePos;
    prevMousePos = newPos;
    return diff;
}

void KeyStateManager::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto &stateMap = getInstance()->getMap();

    if (action == GLFW_PRESS) {
        stateMap[key] = true;
    } else if (action == GLFW_RELEASE) {
        stateMap[key] = false;
    }
}

void KeyStateManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    auto &stateMap = getInstance()->getMap();

    if (action == GLFW_PRESS) {
        stateMap[button] = true;
    } else if (action == GLFW_RELEASE) {
        stateMap[button] = false;
    }

    if (getInstance()->prevMouseCallbackSet) {
        getInstance()->prevMouseCallback(window, button, action, mods);
    }
}

std::unordered_map<int, bool> &KeyStateManager::getMap() {
    return stateMap;
}

bool KeyStateManager::getKeyState(int key) {
    return stateMap.count(key) && stateMap[key];
}

void KeyStateManager::registerGLFW(GLFWwindow *window) {
    glfwSetKeyCallback(window, glfwKeyCallback);
    prevMouseCallback = glfwSetMouseButtonCallback(window, mouseButtonCallback);
    prevMouseCallbackSet = true;
}
