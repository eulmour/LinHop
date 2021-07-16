#include "Renderer.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}
void GLAPIENTRY errorOccurredGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	printf("Message from OpenGL:\nSource: 0x%x\nType: 0x%x\n"
		"Id: 0x%x\nSeverity: 0x%x\n", source, type, id, severity);
	printf("%s\n", message);
	exit(-1); // shut down the program gracefully (it does cleanup stuff too)
  // without exit(), OpenGL will constantly print the error message... make sure to kill your program.
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader)
{
	shader.Bind();
	//shader.SetUniform4f("u_Color", 0.4f, 0.3f, 0.8f, 1.0f);
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawArrays(const VertexArray& va, const VertexBuffer& vb, Shader& shader)
{
	shader.Bind();
	va.Bind();
	vb.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, vb.GetCount()));
}
