#pragma once

#include <string>
#include <stdexcept>
#include <utility>
#include "GL/glew.h"

enum class ShaderType {
    FRAGMENT_SHADER,
    VERTEX_SHADER,
    COMPUTE_SHADER
    // in the might future might add other types such as geometry, tesselation etc.
};

inline GLenum ShaderTypeToGlType(const ShaderType &type) {
    switch (type) {
        case ShaderType::FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
        case ShaderType::VERTEX_SHADER: return GL_VERTEX_SHADER;
        case ShaderType::COMPUTE_SHADER: return GL_COMPUTE_SHADER;
        default: throw std::runtime_error("Unknown Shader Type");
    }
}

typedef struct Shader_Definition_s {
    std::string path;
    ShaderType type;

    Shader_Definition_s(std::string path, ShaderType type) {
        this->path = std::move(path);
        this->type = type;
    };
} ShaderDefinition;

class Shader {
    unsigned int mGLid;
    ShaderDefinition m_ShaderDef;
    ShaderType type;

    bool mIsValid = false;
public:
    Shader(ShaderDefinition definition);

    void destroy();

    unsigned int getGlId();

    void reloadShader();
    bool isValid() { return mIsValid; }
};
