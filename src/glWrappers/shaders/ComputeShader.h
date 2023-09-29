
#ifndef RAYTRACER_COMPUTESHADER_H
#define RAYTRACER_COMPUTESHADER_H


#include <string>
#include "GL/glew.h"

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

};


#endif //RAYTRACER_COMPUTESHADER_H
