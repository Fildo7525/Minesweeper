#include "Tile.h"

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define BLACK_COLOR ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define GRAY_COLOR ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
#define GREEN_COLOR (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f)

Tile::Tile(Icon::Ocupant ocupant, Pose position, bool clicked)
	: m_ocupant(ocupant)
	, m_clicked(clicked)
	, m_position(position)
{
}

bool Tile::belongsToOponent(const Icon::Ocupant &player) const
{
	return m_ocupant != player && m_ocupant != Icon::Ocupant::Empty;
}

bool Tile::belongsToUs(const Icon::Ocupant &player) const
{
	return m_ocupant == player;
}

Tile& Tile::click(bool c)
{
	m_clicked = c;
	return *this;
}

ImColor Tile::color() const
{
	ImColor red(1.0f, 0.0f, 0.0f, 1.0f);
	if (!m_clicked) {
		return GREEN_COLOR;
	}
	if (m_ocupant == Icon::Ocupant::Empty) {
		return BLACK_COLOR;
	}

	if (m_ocupant == Icon::Ocupant::Flag) {
		return GRAY_COLOR;
	}

	if (m_ocupant == Icon::Ocupant::Mine) {
		return GRAY_COLOR;
	}

	return GREEN_COLOR;
}

Icon::Ocupant Tile::ocupant() const
{
	return m_ocupant;
}
