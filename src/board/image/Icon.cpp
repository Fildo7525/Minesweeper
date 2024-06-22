#include "Icon.h"
#include "Image.h"
#include "imgui.h"

Icon::Icon(Ocupant ocupation, const std::string &texturePath, int width, int height)
	: m_ocupation(ocupation)
	, m_width(width)
	, m_height(height)
	, m_texturePath(texturePath)
{
	int h;
	int w;

	LoadTextureFromFile(m_texturePath.c_str(), &m_texture, &w, &h);
}

Icon::Ocupant Icon::ocupation() const
{
	return m_ocupation;
}

GLuint Icon::texture()
{
	return m_texture;
}

