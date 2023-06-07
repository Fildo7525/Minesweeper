#include "Tile.h"

Tile::Tile(std::shared_ptr<Player> ocupant, ImColor color)
	: m_ocupant(ocupant)
	, m_color(color)
{
}

bool Tile::belongsToOponent(const std::shared_ptr<Player> &player) const
{
	return m_ocupant != player && m_ocupant->ocupation() != Player::Ocupant::Empty;
}

bool Tile::belongsToUs(const std::shared_ptr<Player> &player) const
{
	return m_ocupant == player;
}

Tile &Tile::setColor(const ImColor &color)
{
	m_color = color;
	return *this;
}

std::shared_ptr<Player> Tile::ocupant() const
{
	return m_ocupant;
}
