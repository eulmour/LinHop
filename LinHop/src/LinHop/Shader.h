#pragma once
#include <string>
#include <unordered_map>
#include "../glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
};

class Shader
{
public:
	Shader() {}
	Shader(const std::string& filepath);
	~Shader();

	Shader& Bind();
	void Unbind() const;

	//Set uniforms
	void SetUniform1i(const std::string& name, const int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, glm::mat4& matrix);

	unsigned int GetID() const { return m_RendererID; }
private:
	std::string m_FilePath;
	unsigned int m_RendererID = 0;
	std::unordered_map<std::string, int> m_UniformLocationCache;
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
	//caching for uniforms
};

