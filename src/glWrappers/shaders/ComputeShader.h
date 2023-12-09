
#ifndef RAYTRACER_COMPUTESHADER_H
#define RAYTRACER_COMPUTESHADER_H


#include <string>
#include "GL/glew.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class ComputeShader {

private:
    GLuint mGLid;

    std::string mSourcePath;
    bool mIsValid;

public:
    ComputeShader(std::string path);

    void bind();
    void unbind();

    void reloadShader();
    void createShader(const std::string &shaderSource);

    bool setUniform1i(const std::string& name, const int value);
    bool setUniform1ui(const std::string& name, const unsigned int value);

    bool setUniform1f(const std::string& name, const float& value);
    bool setUniform2f(const std::string& name, const glm::vec2& value);
    bool setUniform3f(const std::string& name, const glm::vec3& value);

private:
    int getUniformLocation(const std::string name);

    void printUniformNotFound(std::string name);
};


#endif //RAYTRACER_COMPUTESHADER_H
