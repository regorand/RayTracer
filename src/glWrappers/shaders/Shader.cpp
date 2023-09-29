#include "Shader.h"

#include "utils/glewIncludeUtils.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/fileUtils.hpp"
#include "ShaderUtils.hpp"

#include <vector>

Shader::Shader(std::string vertPath, std::string fragPath)
{
	m_ShaderDef.vertex_path = vertPath;
	m_ShaderDef.fragment_path = fragPath;

	reloadShader();
}

Shader::Shader(ShaderDefinition shaderDef)
	: m_ShaderDef(shaderDef)
{
	reloadShader();
}

Shader::~Shader()
{
	glDeleteProgram(mGLid);
}

unsigned int Shader::getUniformLocation(const std::string name)
{
	auto name_c_str = name.c_str();
	unsigned int res = glGetUniformLocation(mGLid, name_c_str);
	return res;
}

void Shader::printUniformNotFound(std::string name)
{
    std::cout << "Could not find Uniform with name \"" << name << "\"" << std::endl;
}

void Shader::reloadShader()
{
	std::string vertSrc = read_file(m_ShaderDef.vertex_path);
	std::string fragSrc = read_file(m_ShaderDef.fragment_path);
	createShader(vertSrc, fragSrc);
	if (mIsValid) {
		glUseProgram(mGLid);
	}
	unbind();
}

void Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    mGLid = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

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

void Shader::bind()
{
	glUseProgram(mGLid);
}

void Shader::unbind()
{
	glUseProgram(0);
}

bool Shader::setUniform1i(const std::string& name, const unsigned int value)
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

bool Shader::setUniform1f(const std::string& name, const float value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform1f(uniformId, value);
	return true;
}

bool Shader::setUniform2f(const std::string& name, const glm::vec2& value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform2f(uniformId, value.x, value.y);
	return true;
}

bool Shader::setUniform3f(const std::string& name, const glm::vec3& value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform3f(uniformId, value.x, value.y, value.z);
	return true;
}

bool Shader::setUniform4f(const std::string& name, const glm::vec4 &value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniform4f(uniformId, value.x, value.y, value.z, value.w);
	return true;
}

bool Shader::setUniformMat4f(const std::string& name, const glm::mat4 &value)
{
	int uniformId = getUniformLocation(name);
	if (uniformId == -1) {
		printUniformNotFound(name);
		return false;
	}
	glUniformMatrix4fv(uniformId, 1, GL_FALSE, glm::value_ptr(value));
	return true;
}

bool Shader::setUniform3fv(const std::string& name, const unsigned int count, const glm::vec3* data)
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