#include "Icon.h"
#include "Image.h"

std::string Icon::ocupantString(Ocupant ocupant)
{
	switch (ocupant) {
	case Ocupant::Empty:
		return "Empty";
	case Ocupant::One:
		return "One";
	case Ocupant::Two:
		return "Two";
	case Ocupant::Three:
		return "Three";
	case Ocupant::Four:
		return "Four";
	case Ocupant::Five:
		return "Five";
	case Ocupant::Six:
		return "Six";
	case Ocupant::Seven:
		return "Seven";
	case Ocupant::Eight:
		return "Eight";
	case Ocupant::Mine:
		return "Mine";
	case Ocupant::Flag:
		return "Flag";
	case Ocupant::WrongFlag:
		return "WrongFlag";
	}
	return "Empty";
}

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

