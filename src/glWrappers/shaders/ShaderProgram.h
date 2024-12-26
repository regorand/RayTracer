#pragma once

#include <vector>
#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/glm.hpp"

class ShaderProgram {
    unsigned int mGlId;
    std::string mName = "_shader";

    std::vector<ShaderDefinition> mShaderDefs;
    std::vector<Shader> mShaders;

    bool mIsValid;
public:
    ShaderProgram();
    ShaderProgram(std::vector<ShaderDefinition> &shaderDefs);

    void destroy();

    void bind();
    void unbind();

    void setName(const std::string& name);
    std::string &getName();
    void reloadShader();

    bool isValid();

    bool setUniform1i(const std::string& name, const int value);
    bool setUniform1ui(const std::string &name, const unsigned int value);

    bool setUniform1f(const std::string& name, const float value);
    bool setUniform2f(const std::string& name, const glm::vec2& value);
    bool setUniform3f(const std::string& name, const glm::vec3& value);
    bool setUniform4f(const std::string &name, const glm::vec4 &value);

    bool setUniformMat4f(const std::string& name, const glm::mat4 &value);

    bool setUniform3fv(const std::string& name, unsigned int count, const glm::vec3* data);

private:
    int getAndValidateUniformLocation(const std::string &name, bool &res);
    bool validateUniform(int id, const std::string &name);
    void printUniformNotFound(const std::string &name);


    void createProgram();
};
