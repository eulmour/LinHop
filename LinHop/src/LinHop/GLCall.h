#pragma once
#include <GL/glew.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define SCROLL(point) { point.x, point.y - scroll}
#define CX (static_cast<float>(Info.width) / 2) /* center x */
#define CY (static_cast<float>(Info.height) / 2) /* center y */

inline struct _Info
{
	unsigned int width;
	unsigned int height;
} Info{ 480U, 720U };

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
void GLAPIENTRY errorOccurredGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam);
