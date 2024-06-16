#include "Tile.h"

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define BLACK_COLOR ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
#define GRAY_COLOR ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
#define GREEN_COLOR (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f)

Tile::Tile(std::shared_ptr<Icon> ocupant, Pose position, bool clicked)
	: m_ocupant(ocupant)
	, m_clicked(clicked)
	, m_position(position)
{
}

bool Tile::belongsToOponent(const std::shared_ptr<Icon> &player) const
{
	return m_ocupant != player && m_ocupant->ocupation() != Icon::Ocupant::Empty;
}

bool Tile::belongsToUs(const std::shared_ptr<Icon> &player) const
{
	return m_ocupant == player;
}

ImColor Tile::color() const
{
	ImColor red(1.0f, 0.0f, 0.0f, 1.0f);
	if (!m_clicked) {
		return GREEN_COLOR;
	}
	if (m_ocupant->ocupation() == Icon::Ocupant::Empty) {
		return BLACK_COLOR;
	}

	if (m_ocupant->ocupation() == Icon::Ocupant::Flag) {
		return GRAY_COLOR;
	}

	if (m_ocupant->ocupation() == Icon::Ocupant::Mine) {
		return GRAY_COLOR;
	}

	return GREEN_COLOR;
}

std::shared_ptr<Icon> Tile::ocupant() const
{
	return m_ocupant;
}
