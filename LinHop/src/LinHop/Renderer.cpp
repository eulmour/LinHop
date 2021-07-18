#include "Renderer.h"
#include "GLCall.h"
#include <iostream>

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

Renderer::Renderer()
{
	_va = new VertexArray();
	_layout = new VertexBufferLayout();

	//float vertices[] = {
	//	// pos      // tex
	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, 0.0f,

	//	0.0f, 1.0f, 0.0f, 1.0f,
	//	1.0f, 1.0f, 1.0f, 1.0f,
	//	1.0f, 0.0f, 1.0f, 0.0f
	//};

	//_vb = new VertexBuffer(vertices, sizeof(vertices));
	//this->layout.Push<float>(2);
	//va.AddBuffer(*_vb, layout);
	//va.Unbind();

	float positions[] =
	{
	-0.5f, -0.5f,	/*Texture coordinates*/	0.0f, 0.0f,	//0
	0.5f, -0.5f,	/*Texture coordinates*/	1.0f, 0.0f,	//1
	0.5f, 0.5f,		/*Texture coordinates*/	1.0f, 1.0f,	//2
	-0.5f, 0.5f,	/*Texture coordinates*/	0.0f, 1.0f	//3
	};

	unsigned int indices[] =
	{
	0, 1, 2,
	2, 3, 0
	};

	_vb = new VertexBuffer(positions, 4 * 4 * sizeof(float));
	_layout->Push<float>(2);
	_layout->Push<float>(2);
	_va->AddBuffer(*_vb, *_layout);

	_ib = new IndexBuffer(indices, 6);

	_va->Unbind();
	_vb->Unbind();
	_ib->Unbind();
}

Renderer::~Renderer()
{
	delete _vb;
	delete _va;
	delete _ib;
	delete _layout;
}

void Renderer::Use(Shader* shader)
{
	_shaderPtr = shader;
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::_drawElements(const VertexArray& va, const IndexBuffer& ib, Shader& shader)
{
	shader.Bind();
	//shader.SetUniform4f("u_Color", 0.4f, 0.3f, 0.8f, 1.0f);
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::_drawArrays(const VertexArray& va, const VertexBuffer& vb, Shader& shader)
{
	shader.Bind();
	va.Bind();
	vb.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, vb.GetCount()));
}

void Renderer::DrawTexture(
	Texture			texture,
	glm::vec2		position,
	glm::vec2		size,
	float			rotate,
	glm::vec3		color)
{
	glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

	_shaderPtr->Bind();
	_shaderPtr->SetUniformMat4f("u_MVP", proj);
	//_shaderPtr->SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 0.0f);

	// prepare transformations
	//_shaderPtr->Bind();
	_va->Bind();
	//_vb->Bind();
	_ib->Bind();

	texture.Bind();
	_shaderPtr->SetUniform1i("u_Texture", 0);

	//GLCall(glDrawArrays(GL_TRIANGLES, 0, _vb->GetCount()));
	GLCall(glDrawElements(GL_TRIANGLES, _ib->GetCount(), GL_UNSIGNED_INT, nullptr));
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(position, 0.0f));

	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	//model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	//model = glm::scale(model, glm::vec3(size, 1.0f));

	//_shaderPtr->SetUniformMat4f("model", model);

	// render textured quad
	//_shaderPtr->SetUniform3f("spriteColor", color.x, color.y, color.z);

	//glActiveTexture(GL_TEXTURE0);
	//texture.Bind();

	//va.Bind();
	//_vb->Bind();
	//glBindVertexArray(va.GetID());
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
}
