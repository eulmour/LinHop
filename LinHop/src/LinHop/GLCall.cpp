#include "GLCall.h"
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