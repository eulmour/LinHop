#pragma once
#include "VertexBuffer.h"
//#include "VertexBufferLayout.h"

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, VertexBufferLayout& layout);
	void Bind() const;
	void Unbind() const;
private:
	unsigned int m_RendererID;
};

