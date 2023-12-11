#pragma once

#include <unordered_map>
#include "GLFW/glfw3.h"
#include "glm/vec2.hpp"

class KeyStateManager {

private:
    static KeyStateManager *_instance;

    std::unordered_map<int, bool> stateMap;

    glm::vec2 prevMousePos;

    void (*prevMouseCallback)(GLFWwindow* window, int button, int action, int mods);
    bool prevMouseCallbackSet = false;
public:
    static KeyStateManager * getInstance();

    static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    bool getKeyState(int key);

    void registerGLFW(GLFWwindow *window);

    glm::vec2 getMouseAndGetDiff(GLFWwindow *window);

private:
    KeyStateManager();

    std::unordered_map<int, bool> &getMap();
};
