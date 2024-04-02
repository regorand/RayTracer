#pragma once

#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "ShaderProgram.h"


inline std::string getShaderTypeString(unsigned int type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
        case GL_COMPUTE_SHADER:
            return "compute";
        default:
            return "unknown";
    }
}
inline ShaderProgram makeComputeProgram(const std::string &computePath, const std::string& name = "") {
    std::vector<ShaderDefinition> shaderDefs;

    shaderDefs.emplace_back( computePath, ShaderType::COMPUTE_SHADER );

    ShaderProgram program(shaderDefs);
    program.setName(name);

    return program;
}

inline ShaderProgram makeRasterizeProgram(const std::string &vertPath, const std::string &fragPath, const std::string& name = "") {
    std::vector<ShaderDefinition> shaderDefs;

    shaderDefs.emplace_back( vertPath, ShaderType::VERTEX_SHADER );
    shaderDefs.emplace_back( fragPath, ShaderType::FRAGMENT_SHADER );

    ShaderProgram program(shaderDefs);
    program.setName(name);

    return program;
}

inline unsigned int     compileShader(unsigned int type, const std::string &source, const std::string &name, bool &compilationResult) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int res;
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char *message = (char *) malloc(length * sizeof(char));

        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile " << getShaderTypeString(type) << ": " << name << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);

        free(message);

        compilationResult = false;
        return 0;
    }

    compilationResult = true;
    return id;
}

inline bool checkLinkStatus(unsigned int GLid) {
    GLint isLinked = 0;
    glGetProgramiv(GLid, GL_LINK_STATUS, &isLinked);

    if (isLinked) return true;

    GLint maxLength = 0;
    glGetProgramiv(GLid, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(GLid, maxLength, &maxLength, &infoLog[0]);

    std::string s(infoLog.begin(), infoLog.end());
    std::cout << "Info Log: " << s << std::endl;

    return false;
}

