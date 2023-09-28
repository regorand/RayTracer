#ifndef RAYTRACER_APPLICATION_H
#define RAYTRACER_APPLICATION_H

#include "utils/glewIncludeUtils.h"
#include <GLFW/glfw3.h>

class Application {
private:
    GLFWwindow* window;

public:
    Application();

    int run();
};


#endif //RAYTRACER_APPLICATION_H
