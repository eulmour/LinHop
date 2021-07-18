#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

#include "Shader.h"
#include "Texture.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Use(Shader* shader);
	void Clear() const;
	void Draw();
	void DrawTexture(
		Texture			texture,
		glm::vec2		position,
		glm::vec2		size = glm::vec2(10.0f, 10.0f),
		float			rotate = 0.0f,
		glm::vec3		color = glm::vec3(1.0f));

private:
	VertexBuffer*		_vb = nullptr;
	VertexBufferLayout* _layout = nullptr;
	VertexArray*		_va = nullptr;
	IndexBuffer*		_ib = nullptr;
	Shader*				_shaderPtr = nullptr;

	void _drawElements(const VertexArray& va, const IndexBuffer& ib, Shader& shader);
	void _drawArrays(const VertexArray& va, const VertexBuffer& vb, Shader& shader);
};

#endif