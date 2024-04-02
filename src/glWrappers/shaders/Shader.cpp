#include "Shader.h"
#include "ShaderLoad.h"
#include "ShaderUtils.hpp"
#include "utils/glewIncludeUtils.h"

#include <utility>

Shader::Shader(ShaderDefinition definition)
    : m_ShaderDef(std::move(definition)), type(m_ShaderDef.type)
{
    reloadShader();
}

void Shader::reloadShader() {
    std::string src = loadShaderSource(m_ShaderDef.path, true);
    bool res;
    mGLid = compileShader(ShaderTypeToGlType(type), src, m_ShaderDef.path, res);
    GLPrintError();
    if (res) {
        mIsValid = true;
    } else {
        mIsValid = false;
    }
}

unsigned int Shader::getGlId() {
    return mGLid;
}

void Shader::destroy() {
    if (isValid()) {
        glDeleteShader(mGLid);
    }
}
