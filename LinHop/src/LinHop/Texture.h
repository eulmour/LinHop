#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
public:
	Texture() {}
	Texture(const std::string &path);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline unsigned int GetID() const	{ return m_RendererID; }
	inline int GetWidth() const			{ return m_Width; }
	inline int GetHeight() const		{ return m_Height; }
	inline int GetBPP() const			{ return m_BPP; }

private:
	unsigned int	m_RendererID = 0;
	std::string		m_FilePath;
	unsigned char*	m_LocalBuffer = nullptr;
	int				m_Width = 0, m_Height = 0, m_BPP = 0;
};

#endif