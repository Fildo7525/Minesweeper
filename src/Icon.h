#pragma once

#include <string>
#include <memory>
#include <GL/gl.h>

class Icon
{
public:
	using Ptr = std::shared_ptr<Icon>;

	enum class Ocupant {
		Empty,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Mine,
		Flag
	};

	explicit Icon(Ocupant ocupation, const std::string &texturePath, int width, int height);
	Ocupant ocupation() const;
	GLuint texture();
	int textureWidth() const { return m_width; }
	int textureHeight() const { return m_height; }

private:
	Ocupant m_ocupation;
	int m_width;
	int m_height;
	GLuint m_texture;
	const std::string m_texturePath;
};
