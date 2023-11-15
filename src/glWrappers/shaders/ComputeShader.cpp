
#include "ComputeShader.h"
#include "utils/fileUtils.hpp"
#include "ShaderUtils.hpp"
#include "ShaderLoad.h"

ComputeShader::ComputeShader(std::string path)
        : mSourcePath(path), mIsValid(false) {
    reloadShader();
}

void ComputeShader::bind()
{
    glUseProgram(mGLid);
}

void ComputeShader::unbind()
{
    glUseProgram(0);
}

void ComputeShader::reloadShader() {

    std::string src = loadShaderSource(mSourcePath, true);
    createShader(src);
    if (mIsValid) {
        glUseProgram(mGLid);
    }
    unbind();
}

void ComputeShader::createShader(const std::string &shaderSource) {
    unsigned int shaderId = compileShader(GL_COMPUTE_SHADER, shaderSource);

    if (shaderId == 0) {
        mIsValid = false;
        return;
    }

    mGLid = glCreateProgram();

    glAttachShader(mGLid, shaderId);
    glLinkProgram(mGLid);

    if (checkLinkStatus(mGLid)) {
        glValidateProgram(mGLid);
        mIsValid = true;
    } else {
        glDeleteProgram(mGLid);
        mIsValid = false;
    }

    glDeleteShader(shaderId);
}

bool ComputeShader::setUniform2f(const std::string &name, const glm::vec2 &value) {
    int uniformId = getUniformLocation(name);
    if (uniformId == -1) {
        printUniformNotFound(name);
        return false;
    }
    glUniform2f(uniformId, value.x, value.y);
    return true;
}

bool ComputeShader::setUniform1f(const std::string &name, const float &value) {
    int uniformId = getUniformLocation(name);
    if (uniformId == -1) {
        printUniformNotFound(name);
        return false;
    }
    glUniform1f(uniformId, value);
    return true;
}

bool ComputeShader::setUniform1ui(const std::string &name, const unsigned int value) {
    int uniformId = getUniformLocation(name);
    if (uniformId == -1) {
        printUniformNotFound(name);
        return false;
    }
    glUniform1ui(uniformId, value);
    return true;
}

int ComputeShader::getUniformLocation(const std::string name) {
    auto name_c_str = name.c_str();
    int res = glGetUniformLocation(mGLid, name_c_str);
    return res;
}

void ComputeShader::printUniformNotFound(std::string name) {
    std::cout << "Could not find Uniform with name \"" << name << "\"" << std::endl;
}

bool ComputeShader::setUniform1i(const std::string &name, const int value) {
    int uniformId = getUniformLocation(name);
    if (uniformId == -1) {
        printUniformNotFound(name);
        return false;
    }
    glUniform1i(uniformId, value);
    return true;
}
