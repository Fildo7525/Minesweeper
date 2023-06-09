#include "Tile.h"

Tile::Tile(std::shared_ptr<Icon> ocupant, ImColor color, bool clicked)
	: m_ocupant(ocupant)
	, m_color(color)
	, m_clicked(clicked)
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

Tile &Tile::setColor(const ImColor &color)
{
	m_color = color;
	return *this;
}

std::shared_ptr<Icon> Tile::ocupant() const
{
	return m_ocupant;
}
