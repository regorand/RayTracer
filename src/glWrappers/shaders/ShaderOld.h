#pragma once

#include <string>
#include <iostream>
#include "glm/glm.hpp"

// TODO ShaderOld Class Rework
/* 1. only single class for a single shader
 * 2. ShaderProgram class that can contain multiple shaders (can I make this exchangable with ShaderOld ? -> Bindable or smth ?)
 * 3. ShaderDefinitionOld struct can be used better -> pair of path and type
 * 3.5 Also have Program Definition
 * 4. Better way to do the uniforms -> either hardcoded list/map of opengl functions, which are passed to general function or one function each or other way ?
 * 5. anything else ?
 *
 */


typedef struct Shader_Definition_old_s {
	std::string vertex_path;
	std::string fragment_path;
} ShaderDefinitionOld;

class ShaderOld
{
private:
	unsigned int mGLid;
	std::string vertPath = "";
	std::string fragPath = "";
	ShaderDefinitionOld m_ShaderDef;

	bool mIsValid = false;
public:
	ShaderOld(std::string vertPath, std::string fragPath);

	ShaderOld(ShaderDefinitionOld m_ShaderDef);
	~ShaderOld();

	void bind();
	void unbind();
	
	bool setUniform1i(const std::string& name, const unsigned int value);

	bool setUniform1f(const std::string& name, const float value);
	bool setUniform2f(const std::string& name, const glm::vec2& value);
	bool setUniform3f(const std::string& name, const glm::vec3& value);
	bool setUniform4f(const std::string &name, const glm::vec4 &value);

	bool setUniformMat4f(const std::string& name, const glm::mat4 &value);

	bool setUniform3fv(const std::string& name, const unsigned int count, const glm::vec3* data);

	void reloadShader();
	bool isValid() { return mIsValid; }
private:
	unsigned int getUniformLocation(const std::string name);
	void printUniformNotFound(std::string name);

	void createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string &vertName, const std::string &fragName);
};

