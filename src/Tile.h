#pragma once

#include "Player.h"
#include "imgui.h"

#include <memory>

class Tile
{
public:
	explicit Tile(std::shared_ptr<Player> ocupant, ImColor color);

	void setOcupant(const std::shared_ptr<Player> &ocupant) { m_ocupant = ocupant; }
	bool belongsToOponent(const std::shared_ptr<Player> &player) const;
	bool belongsToUs(const std::shared_ptr<Player> &player) const;

	ImColor color() const { return m_color; }
	Tile &setColor(const ImColor &color);
	bool selected() const { return m_ocupant->ocupation() != Player::Ocupant::Empty; }
	std::shared_ptr<Player> ocupant() const;
private:
	ImColor m_color;
	std::shared_ptr<Player> m_ocupant;
};
