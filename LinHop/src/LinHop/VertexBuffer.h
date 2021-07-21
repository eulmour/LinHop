#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

class VertexBuffer
{
public:
	VertexBuffer(const void* data, unsigned int size, bool bDynamic = false);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	unsigned int GetCount() const;
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
};

#endif