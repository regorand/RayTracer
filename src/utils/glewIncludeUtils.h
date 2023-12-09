#ifndef RAYTRACER_GLEW_INCLUDE_UTILS_H
#define RAYTRACER_GLEW_INCLUDE_UTILS_H

#include "GL/glew.h"

#ifdef _WIN32
#define BREAK_POINT __debugbreak();
#elif defined(unix) || defined(__uni__) || defined(__unix)
// untested
# define BREAK_POINT raise(SIGTRAP);
#endif

#include <GL/glew.h>
#include <iostream>

static void inline GLClearError()
{
    while (glGetError()/* != GL_NO_ERROR */);
}

static void inline GLPrintError() {
    GLenum error;
    while (error = glGetError()/* != GL_NO_ERROR */)
    {
        std::cout << "OpenGL Error: " << error << std::endl;
    }
}

static int logLevel = 1;

void inline GLAPIENTRY callBackFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::string m(message, length);
    int messageLogLevel = -1;
    switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            messageLogLevel = 0;
            break;
        case GL_DEBUG_SEVERITY_LOW:
            BREAK_POINT
            messageLogLevel = 1;
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            BREAK_POINT
            messageLogLevel = 2;
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            BREAK_POINT
            messageLogLevel = 3;
            break;
    }
    if (messageLogLevel >= logLevel)
        std::cout << "GL Message Callback; Level " << messageLogLevel << ": " << m << std::endl;
}

void inline registerGLCallBack() {
    glDebugMessageCallback(callBackFunc, NULL);
}



#endif //RAYTRACER_GLEW_INCLUDE_UTILS_H
