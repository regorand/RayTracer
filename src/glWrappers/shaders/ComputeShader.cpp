
#include "ComputeShader.h"
#include "utils/fileUtils.hpp"
#include "ShaderUtils.hpp"

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

    std::string src = read_file(mSourcePath);
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
