#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "Texture.h"
#include "GameObject.h"

class Renderer
{
public:
	void DrawRect(
		const RectangleObject&	rect,
		glm::vec2				position,
		glm::vec2				size = glm::vec2(10.0f, 10.0f),
		float					rotate = 0.0f,
		glm::vec4				color = glm::vec4(1.0f));

	void DrawLine(
		const LineObject& line,
		glm::vec2		a_pos,
		glm::vec2		b_pos,
		glm::vec4		color);

	void DrawText(
		std::string			text,
		TextObject&	obj,
		glm::vec2			pos,
		glm::vec3			color,
		float				scale
	);
};

#endif