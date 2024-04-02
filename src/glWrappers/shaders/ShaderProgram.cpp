#include "ShaderProgram.h"
#include "ShaderUtils.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils/glewIncludeUtils.h"

ShaderProgram::ShaderProgram()
    : mIsValid(false)
{ }

ShaderProgram::ShaderProgram(std::vector<ShaderDefinition> &shaderDefs)
    : mShaderDefs(shaderDefs)
{
    reload();
}

void ShaderProgram::destroy() {
    if (isValid()) {
        glDeleteProgram(mGlId);

    }

    for (auto &shader: mShaders) {
        shader.destroy();
    }
}

void ShaderProgram::bind() {
    if (isValid()) {
        glUseProgram(mGlId);
    }
}

void ShaderProgram::unbind() {
    glUseProgram(0);
}

void ShaderProgram::setName(const std::string& name) {
    mName = name;
}

void ShaderProgram::reload() {
    std::vector<Shader> newShaders;

    bool no_error = true;
    newShaders.reserve(mShaderDefs.size());
    for (auto &def: mShaderDefs) {
        newShaders.emplace_back(def);
        no_error = no_error && newShaders[newShaders.size() - 1].isValid();
    }
    if (no_error) {
        for (auto &shader: mShaders) {
            shader.destroy();
        }

        mShaders.clear();
        mShaders = std::move(newShaders);

        createProgram();
    } else {
        std::cout << "Shader Reload failed: a shader could not be loaded correctly.\nAborting shader reload." << std::endl;
    }
}

bool ShaderProgram::isValid() {
    return mIsValid;
}

void ShaderProgram::createProgram() {
    mGlId = glCreateProgram();

    for (auto &shader: mShaders) {
        if (!shader.isValid()) {
            mIsValid = false;
        }


    }

    glAttachShader(mGlId, mShaders[0].getGlId());
    GLPrintError();
    glAttachShader(mGlId, mShaders[1].getGlId());
    GLPrintError();

    GLsizei count;
    GLuint shaders[5];
    glGetAttachedShaders(mGlId, 5, &count, shaders);


    glLinkProgram(mGlId);


    if (checkLinkStatus(mGlId)) {
        glValidateProgram(mGlId);
        mIsValid = true;
    } else {
        glDeleteProgram(mGlId);
        mIsValid = false;
    }
}

int ShaderProgram::getAndValidateUniformLocation(const std::string &name, bool &res) {
    if (!isValid()) {
        res = false;
        return -1;
    }
    auto name_c_str = name.c_str();

    int result = glGetUniformLocation(mGlId, name_c_str);

    res = validateUniform(result, name);
    return result;
}

bool ShaderProgram::validateUniform(int id, const std::string &name) {
    if (id == -1) {
        printUniformNotFound(name);
        return false;
    }
    return true;
}

void ShaderProgram::printUniformNotFound(const std::string &name) {
    // TODO better system for debug output
    // std::cout << "Could not find Uniform with name \"" << name << "\"" << std::endl;
}

bool ShaderProgram::setUniform1i(const std::string &name, const unsigned int value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniform1i(uniformId, value);
    }
    return res;
}

bool ShaderProgram::setUniform1f(const std::string &name, const float value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniform1f(uniformId, value);
    }
    return res;
}

bool ShaderProgram::setUniform2f(const std::string &name, const glm::vec2 &value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniform2f(uniformId, value.x, value.y);
    }
    return res;
}

bool ShaderProgram::setUniform3f(const std::string &name, const glm::vec3 &value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniform3f(uniformId, value.x, value.y, value.z);
    }
    return res;
}

bool ShaderProgram::setUniform4f(const std::string &name, const glm::vec4 &value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniform4f(uniformId, value.x, value.y, value.z, value.w);
    }
    return res;
}

bool ShaderProgram::setUniformMat4f(const std::string &name, const glm::mat4 &value) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {
        glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
    }
    return res;
}

bool ShaderProgram::setUniform3fv(const std::string &name, unsigned int count, const glm::vec3 *data) {
    bool res;
    int uniformId = getAndValidateUniformLocation(name, res);
    if (res) {	std::vector<float> raw_data;
        for (unsigned int i = 0; i < count; i++) {
            raw_data.push_back(data[i].x);
            raw_data.push_back(data[i].y);
            raw_data.push_back(data[i].z);
        }
        glUniform3fv(uniformId, count * 3, raw_data.data());
    }
    return res;
}

