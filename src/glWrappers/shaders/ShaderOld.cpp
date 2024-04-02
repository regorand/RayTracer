#include "ShaderOld.h"

#include "utils/glewIncludeUtils.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/fileUtils.hpp"
#include "ShaderUtils.hpp"
#include "ShaderLoad.h"

#include <vector>

ShaderOld::ShaderOld(std::string vertPath, std::string fragPath)
{
	m_ShaderDef.vertex_path = vertPath;
	m_ShaderDef.fragment_path = fragPath;

	reloadShader();
}

ShaderOld::ShaderOld(ShaderDefinitionOld shaderDef)
	: m_ShaderDef(shaderDef)
{
	reloadShader();
}

ShaderOld::~ShaderOld()
{
	glDeleteProgram(mGLid);
}

unsigned int ShaderOld::getUniformLocation(const std::string name)
{
	auto name_c_str = name.c_str();
	unsigned int res = glGetUniformLocation(mGLid, name_c_str);
	return res;
}

void ShaderOld::printUniformNotFound(std::string name)
{
    // TODO better system for debug output
    // std::cout << "Could not find Uniform with name \"" << name << "\"" << std::endl;
}

void ShaderOld::reloadShader()
{
    std::string vertSrc = loadShaderSource(m_ShaderDef.vertex_path, true);
    std::string fragSrc = loadShaderSource(m_ShaderDef.fragment_path, true);
	createShader(vertSrc, fragSrc, m_ShaderDef.vertex_path, m_ShaderDef.fragment_path);
	if (mIsValid) {
		glUseProgram(mGLid);
	}
	unbind();
}

void ShaderOld::createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string &vertName, const std::string &fragName)
{
    mGLid = glCreateProgram();
    bool res;
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader, vertName, res);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader, fragName, res);

	if (vs == 0 || fs == 0) {
        mIsValid = false;
		return;
	}

	glAttachShader(mGLid, vs);
	glAttachShader(mGLid, fs);

	glLinkProgram(mGLid);


    if (checkLinkStatus(mGLid)) {
        glValidateProgram(mGLid);
        mIsValid = true;
    } else {
        glDeleteProgram(mGLid);
        mIsValid = false;
    }

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ShaderOld::bind()
{
	glUseProgram(mGLid);
}

void ShaderOld::unbind()
{
	glUseProgram(0);
}

bool ShaderOld::setUniform1i(const std::string& name, const unsigned int value)
{
	GLint isLinked = 0;
	glGetProgramiv(mGLid, GL_LINK_STATUS, &isLinked);
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform1i(uniformId, value);
	return true;
}

bool ShaderOld::setUniform1f(const std::string& name, const float value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform1f(uniformId, value);
	return true;
}

bool ShaderOld::setUniform2f(const std::string& name, const glm::vec2& value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform2f(uniformId, value.x, value.y);
	return true;
}

bool ShaderOld::setUniform3f(const std::string& name, const glm::vec3& value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform3f(uniformId, value.x, value.y, value.z);
	return true;
}

bool ShaderOld::setUniform4f(const std::string& name, const glm::vec4 &value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform4f(uniformId, value.x, value.y, value.z, value.w);
	return true;
}

bool ShaderOld::setUniformMat4f(const std::string& name, const glm::mat4 &value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

bool ShaderOld::setUniform3fv(const std::string& name, const unsigned int count, const glm::vec3* data)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	std::vector<float> raw_data;
	for (unsigned int i = 0; i < count; i++) {
		raw_data.push_back(data[i].x);
		raw_data.push_back(data[i].y);
		raw_data.push_back(data[i].z);
	}
	glUniform3fv(uniformId, count * 3, raw_data.data());
	return true;
}