#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout);
	unsigned int GetID() const { return m_RendererID; }
	void Bind() const;
	void Unbind() const;
private:
	unsigned int m_RendererID;
};

#endif