#pragma once

#include "Icon.h"
#include "imgui.h"

#include <memory>

struct Pose {
	int x;
	int y;

};

bool operator==(const Pose &lhs, const Pose &rhs);

namespace std
{
	template <>
	struct hash<Pose>
	{
		size_t operator()(Pose const &pose) const noexcept
		{
			return std::hash<int>{}(pose.x) ^ (std::hash<int>{}(pose.y) << 1);
		}
	};
}


class Tile
{
public:
	explicit Tile(std::shared_ptr<Icon> ocupant, ImColor color, Pose position, bool clicked = false);

	bool belongsToOponent(const std::shared_ptr<Icon> &player) const;
	bool belongsToUs(const std::shared_ptr<Icon> &player) const;

	Tile &setOcupant(const std::shared_ptr<Icon> &ocupant) { m_ocupant = ocupant; return *this; }

	Pose position() const { return m_position; }
	bool clicked() const { return m_clicked; }
	Tile &click(bool c = true) { m_clicked = c; return *this; }

	ImColor color() const { return m_color; }
	Tile &setColor(const ImColor &color);
	bool selected() const { return m_ocupant->ocupation() != Icon::Ocupant::Empty; }
	std::shared_ptr<Icon> ocupant() const;
private:
	ImColor m_color;
	std::shared_ptr<Icon> m_ocupant;
	bool m_clicked;
	Pose m_position;
};
